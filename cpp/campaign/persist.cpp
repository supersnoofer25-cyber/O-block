// Port of campaign/persist.go. Go leans on encoding/json from the standard library;
// C++'s standard library has no equivalent, and ADR 0014 chose this port specifically
// to avoid taking on a language boundary and its dependencies for the rest of the
// project's life. So this file carries a JSON value type and a parser/writer sized
// for exactly one job — round-tripping State — rather than reaching for a general
// third-party library the way the rest of this module deliberately does not.
//
// Scope is narrower than the JSON spec on purpose: no \u escapes in or out, because
// this writer never produces one and nothing this module saves needs one (raw UTF-8
// bytes above 0x7F, like the é in flare.cpp's bystanders, are valid JSON string
// content without escaping). A parser that has to accept arbitrary external JSON
// would need more; a parser that only ever reads what Save just wrote does not.
#include "campaign.hpp"

#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <system_error>
#include <variant>

namespace campaign {

namespace {

struct JsonValue;
using JsonArray = std::vector<JsonValue>;
using JsonObject = std::vector<std::pair<std::string, JsonValue>>;

struct JsonValue {
  std::variant<std::monostate, bool, long long, std::string, JsonArray, JsonObject> v;

  JsonValue() = default;
  JsonValue(bool b) : v(b) {}
  JsonValue(long long n) : v(n) {}
  JsonValue(int n) : v(static_cast<long long>(n)) {}
  JsonValue(std::string s) : v(std::move(s)) {}
  JsonValue(JsonArray a) : v(std::move(a)) {}
  JsonValue(JsonObject o) : v(std::move(o)) {}
};

// --- writing -----------------------------------------------------------------

void WriteString(std::string& out, const std::string& s) {
  out.push_back('"');
  for (char c : s) {
    switch (c) {
      case '"':
        out += "\\\"";
        break;
      case '\\':
        out += "\\\\";
        break;
      case '\n':
        out += "\\n";
        break;
      case '\r':
        out += "\\r";
        break;
      case '\t':
        out += "\\t";
        break;
      default:
        out.push_back(c);
    }
  }
  out.push_back('"');
}

void WriteValue(std::string& out, const JsonValue& jv) {
  struct Visitor {
    std::string& out;
    void operator()(std::monostate) const { out += "null"; }
    void operator()(bool b) const { out += b ? "true" : "false"; }
    void operator()(long long n) const { out += std::to_string(n); }
    void operator()(const std::string& s) const { WriteString(out, s); }
    void operator()(const JsonArray& a) const {
      out.push_back('[');
      for (size_t i = 0; i < a.size(); ++i) {
        if (i) out.push_back(',');
        WriteValue(out, a[i]);
      }
      out.push_back(']');
    }
    void operator()(const JsonObject& o) const {
      out.push_back('{');
      for (size_t i = 0; i < o.size(); ++i) {
        if (i) out.push_back(',');
        WriteString(out, o[i].first);
        out.push_back(':');
        WriteValue(out, o[i].second);
      }
      out.push_back('}');
    }
  };
  std::visit(Visitor{out}, jv.v);
}

std::string Write(const JsonValue& jv) {
  std::string out;
  WriteValue(out, jv);
  return out;
}

// --- parsing -------------------------------------------------------------------

class Parser {
 public:
  explicit Parser(const std::string& s) : s_(s), i_(0) {}

  bool Parse(JsonValue& out) {
    SkipWs();
    if (!ParseValue(out)) return false;
    SkipWs();
    return true;
  }

 private:
  const std::string& s_;
  size_t i_;

  void SkipWs() {
    while (i_ < s_.size() && (s_[i_] == ' ' || s_[i_] == '\t' || s_[i_] == '\n' ||
                               s_[i_] == '\r')) {
      ++i_;
    }
  }

  bool ParseValue(JsonValue& out) {
    if (i_ >= s_.size()) return false;
    switch (s_[i_]) {
      case '"':
        return ParseString(out);
      case '{':
        return ParseObject(out);
      case '[':
        return ParseArray(out);
      case 't':
        if (s_.compare(i_, 4, "true") == 0) {
          i_ += 4;
          out = JsonValue(true);
          return true;
        }
        return false;
      case 'f':
        if (s_.compare(i_, 5, "false") == 0) {
          i_ += 5;
          out = JsonValue(false);
          return true;
        }
        return false;
      case 'n':
        if (s_.compare(i_, 4, "null") == 0) {
          i_ += 4;
          out = JsonValue();
          return true;
        }
        return false;
      default:
        return ParseNumber(out);
    }
  }

  bool ParseString(JsonValue& out) {
    std::string raw;
    if (!ParseRawString(raw)) return false;
    out = JsonValue(std::move(raw));
    return true;
  }

  bool ParseRawString(std::string& out) {
    if (i_ >= s_.size() || s_[i_] != '"') return false;
    ++i_;
    out.clear();
    while (i_ < s_.size() && s_[i_] != '"') {
      char c = s_[i_++];
      if (c == '\\' && i_ < s_.size()) {
        char esc = s_[i_++];
        switch (esc) {
          case '"':
            out.push_back('"');
            break;
          case '\\':
            out.push_back('\\');
            break;
          case '/':
            out.push_back('/');
            break;
          case 'n':
            out.push_back('\n');
            break;
          case 'r':
            out.push_back('\r');
            break;
          case 't':
            out.push_back('\t');
            break;
          default:
            out.push_back(esc);
        }
      } else {
        out.push_back(c);
      }
    }
    if (i_ >= s_.size()) return false;  // unterminated string
    ++i_;                               // closing quote
    return true;
  }

  bool ParseNumber(JsonValue& out) {
    size_t start = i_;
    if (i_ < s_.size() && s_[i_] == '-') ++i_;
    while (i_ < s_.size() && std::isdigit(static_cast<unsigned char>(s_[i_]))) ++i_;
    if (i_ == start) return false;
    out = JsonValue(static_cast<long long>(std::stoll(s_.substr(start, i_ - start))));
    return true;
  }

  bool ParseArray(JsonValue& out) {
    ++i_;  // '['
    JsonArray arr;
    SkipWs();
    if (i_ < s_.size() && s_[i_] == ']') {
      ++i_;
      out = JsonValue(std::move(arr));
      return true;
    }
    for (;;) {
      SkipWs();
      JsonValue v;
      if (!ParseValue(v)) return false;
      arr.push_back(std::move(v));
      SkipWs();
      if (i_ >= s_.size()) return false;
      if (s_[i_] == ',') {
        ++i_;
        continue;
      }
      if (s_[i_] == ']') {
        ++i_;
        break;
      }
      return false;
    }
    out = JsonValue(std::move(arr));
    return true;
  }

  bool ParseObject(JsonValue& out) {
    ++i_;  // '{'
    JsonObject obj;
    SkipWs();
    if (i_ < s_.size() && s_[i_] == '}') {
      ++i_;
      out = JsonValue(std::move(obj));
      return true;
    }
    for (;;) {
      SkipWs();
      std::string key;
      if (!ParseRawString(key)) return false;
      SkipWs();
      if (i_ >= s_.size() || s_[i_] != ':') return false;
      ++i_;
      SkipWs();
      JsonValue v;
      if (!ParseValue(v)) return false;
      obj.emplace_back(std::move(key), std::move(v));
      SkipWs();
      if (i_ >= s_.size()) return false;
      if (s_[i_] == ',') {
        ++i_;
        continue;
      }
      if (s_[i_] == '}') {
        ++i_;
        break;
      }
      return false;
    }
    out = JsonValue(std::move(obj));
    return true;
  }
};

const JsonValue* Find(const JsonObject& o, const std::string& key) {
  for (const auto& [k, v] : o) {
    if (k == key) return &v;
  }
  return nullptr;
}

// --- State <-> JsonValue ---------------------------------------------------------

JsonValue ToJson(const std::vector<MemberID>& ids) {
  JsonArray arr;
  arr.reserve(ids.size());
  for (const auto& id : ids) arr.emplace_back(id);
  return JsonValue(std::move(arr));
}

bool FromJsonStrings(const JsonValue& jv, std::vector<MemberID>& out) {
  const auto* arr = std::get_if<JsonArray>(&jv.v);
  if (!arr) return false;
  out.clear();
  for (const auto& e : *arr) {
    const auto* s = std::get_if<std::string>(&e.v);
    if (!s) return false;
    out.push_back(*s);
  }
  return true;
}

JsonValue ToJson(const State& s) {
  JsonObject obj;
  obj.emplace_back("chapter", JsonValue(s.Chapter));
  obj.emplace_back("alive", ToJson(s.Alive));
  obj.emplace_back("outside", ToJson(s.Outside));

  JsonArray fam;
  fam.reserve(s.Familiarity.size());
  for (const auto& f : s.Familiarity) {
    JsonObject fo;
    fo.emplace_back("member", JsonValue(f.Member));
    fo.emplace_back("evenings", JsonValue(f.Evenings));
    fam.emplace_back(std::move(fo));
  }
  obj.emplace_back("familiarity", JsonValue(std::move(fam)));

  obj.emplace_back("cycle", JsonValue(s.Cycle));

  JsonObject lb;
  lb.emplace_back("happened", JsonValue(s.LastBip.Happened));
  lb.emplace_back("lost", JsonValue(s.LastBip.Lost));
  lb.emplace_back("who", JsonValue(s.LastBip.Who));
  obj.emplace_back("last_bip", JsonValue(std::move(lb)));

  const char* end = "";
  switch (s.End) {
    case EndReason::NotEnded:
      end = "";
      break;
    case EndReason::Finished:
      end = "finished";
      break;
    case EndReason::SetEmpty:
      end = "set-empty";
      break;
  }
  obj.emplace_back("end", JsonValue(std::string(end)));

  return JsonValue(std::move(obj));
}

bool FromJson(const JsonValue& jv, State& out) {
  const auto* obj = std::get_if<JsonObject>(&jv.v);
  if (!obj) return false;

  State s;
  if (const auto* v = Find(*obj, "chapter")) {
    if (const auto* n = std::get_if<long long>(&v->v)) s.Chapter = static_cast<int>(*n);
  }
  if (const auto* v = Find(*obj, "alive")) {
    if (!FromJsonStrings(*v, s.Alive)) return false;
  }
  if (const auto* v = Find(*obj, "outside")) {
    if (!FromJsonStrings(*v, s.Outside)) return false;
  }
  if (const auto* v = Find(*obj, "familiarity")) {
    const auto* arr = std::get_if<JsonArray>(&v->v);
    if (!arr) return false;
    for (const auto& e : *arr) {
      const auto* eo = std::get_if<JsonObject>(&e.v);
      if (!eo) return false;
      FamiliarityEntry fe;
      if (const auto* m = Find(*eo, "member")) {
        if (const auto* ms = std::get_if<std::string>(&m->v)) fe.Member = *ms;
      }
      if (const auto* ev = Find(*eo, "evenings")) {
        if (const auto* en = std::get_if<long long>(&ev->v))
          fe.Evenings = static_cast<int>(*en);
      }
      s.Familiarity.push_back(std::move(fe));
    }
  }
  if (const auto* v = Find(*obj, "cycle")) {
    if (const auto* n = std::get_if<long long>(&v->v)) s.Cycle = static_cast<int>(*n);
  }
  if (const auto* v = Find(*obj, "last_bip")) {
    const auto* lo = std::get_if<JsonObject>(&v->v);
    if (!lo) return false;
    if (const auto* h = Find(*lo, "happened")) {
      if (const auto* b = std::get_if<bool>(&h->v)) s.LastBip.Happened = *b;
    }
    if (const auto* l = Find(*lo, "lost")) {
      if (const auto* b = std::get_if<bool>(&l->v)) s.LastBip.Lost = *b;
    }
    if (const auto* w = Find(*lo, "who")) {
      if (const auto* ws = std::get_if<std::string>(&w->v)) s.LastBip.Who = *ws;
    }
  }
  if (const auto* v = Find(*obj, "end")) {
    if (const auto* es = std::get_if<std::string>(&v->v)) {
      if (*es == "finished")
        s.End = EndReason::Finished;
      else if (*es == "set-empty")
        s.End = EndReason::SetEmpty;
      else
        s.End = EndReason::NotEnded;
    }
  }

  out = std::move(s);
  return true;
}

}  // namespace

bool Save(const std::string& dir, const State& s) {
  std::error_code ec;
  std::filesystem::create_directories(dir, ec);
  if (ec) return false;

  std::string body = Write(ToJson(s));
  std::string dest = dir + "/" + SlotName;
  std::string tmp = dest + ".tmp";

  {
    std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
    if (!f) return false;
    f << body;
    if (!f) return false;
  }

  // Atomic via rename, so a crash mid-write cannot leave a half-written campaign —
  // losing a whole campaign to a torn file would be a far worse outcome than any
  // death in it.
  std::error_code rn;
  std::filesystem::rename(tmp, dest, rn);
  return !rn;
}

bool Load(const std::string& dir, State& out) {
  std::ifstream f(dir + "/" + SlotName, std::ios::binary);
  if (!f) return false;
  std::ostringstream buf;
  buf << f.rdbuf();

  std::string content = buf.str();
  JsonValue jv;
  if (!Parser(content).Parse(jv)) return false;
  return FromJson(jv, out);
}

}  // namespace campaign

---
name: grill-with-docs
description: A relentless, one-question-at-a-time interview that stress-tests a plan or design at the very start of a change — before any spec or code — and leaves a paper trail. Sharpens fuzzy language into canonical terms written to a CONTEXT.md glossary the moment each term settles, and records genuinely hard-to-reverse decisions as ADRs under docs/adr/. This is invoked explicitly by the user (typically by typing /grill-with-docs); do not reach for it on your own.
---

# Grill With Docs

You are about to run a **grill**: a relentless, one-question-at-a-time interview that
pins down what the user actually wants to build, before any spec or code exists. The
grilling is not the whole job — as understanding forms, you write it down. Settled
vocabulary lands in a `CONTEXT.md` glossary; the rare, genuinely one-way decisions land
as ADRs. A plain interview evaporates when the session ends. This one leaves the
alignment in the repo, in the project's own words, so it survives the conversation.

Two things are happening at once, and both matter:

1. **The grill** — you interrogate the plan until you and the user share a clear picture.
2. **The paper trail** — you capture terms and decisions *inline*, the moment they
   resolve, not batched at the end.

## When this runs

This skill is user-invoked. The user typed `/grill-with-docs` (or asked for it by name)
because they're at the **start of a change**, the plan is still fuzzy, and the domain
language isn't settled — and they want to stress-test both before writing anything down
as a spec. That's the sweet spot.

Redirect instead of grilling when:

- **The plan is already clear and you only need to nail down terminology** → this is
  `domain-modeling` territory, not a full grill. Just capture the glossary/ADRs.
- **They want the interview but not the artifacts** → that's plain `grilling`. Skip the
  file writes.
- **The change is too big to hold in one session and the route is still foggy** (a
  greenfield project, a huge feature) → start upstream with `wayfinder` to chart the
  effort as a map of decisions first, then come back here.

If a neighboring skill is installed, mention it; if not, just adjust your own behavior
accordingly (e.g. skip the file writes for a plain interview).

## The shape of a session

```
grill one question → resolve it → write any settled term to CONTEXT.md
    → (rarely) write an ADR for a one-way decision
    → next question … → shared understanding reached → hand off toward to-spec
```

Most sessions end with a **sharper glossary and few or no ADRs**. That is the intended
shape — if you're generating a pile of ADRs, you're probably recording reversible
choices that don't warrant one.

This skill is the opening step of a longer chain:

```
grill-with-docs → to-spec → to-tickets → implement → code-review
```

It comes *before* the spec. Your output is the shared understanding and settled
vocabulary that `to-spec` will later synthesize into a spec **without re-interviewing the
user**. So the glossary and ADRs you write are the real deliverable — treat them as
handoff artifacts, not scratch notes.

## The grill (the engine)

The engine is a walk down the plan's decision tree, resolving dependencies before moving
on. Run it like this:

- **One question at a time. Then stop and wait.** Never dump a numbered questionnaire.
  The whole value is that each answer reshapes the next question — you can't get that
  from a batch. Ask, wait for the reply, absorb it, then ask the next.

- **Offer a recommended answer with every question.** The grill is opinionated, not a
  blank interrogation. Lead with your best default and a one-line reason, so the user can
  just say "yes" or redirect. Bad: "How should sharing work?" Good: "You said users can
  'share' a trip — I'd reuse the existing `collaborators` list rather than build a
  separate share-link concept, since it's already wired up. That work for you, or did you
  mean something read-only and public?"

- **Resolve dependencies in order.** Don't ask a downstream question whose very framing
  depends on an unresolved upstream decision. If a choice would change what the next
  three questions even mean, ask that one first.

- **Answer from the codebase before asking the user.** Many questions the repo can settle
  itself — what already exists, how something is currently modeled, what a name refers to.
  Read/grep first. Then *confirm* what you found rather than asking cold: "The plan
  mentions 'the sync job' — that's the cron in `scripts/sync.ts`, right?" Only spend the
  user's attention on what code genuinely can't tell you: intent, priorities,
  product trade-offs, what "good" means to them.

- **Sharpen fuzzy language on the spot.** When the user reaches for a vague or overloaded
  word, propose a precise, canonical term, get agreement, and — the instant it's agreed —
  write it to the glossary (see below). Do this in the project's own words, not generic
  jargon.

- **Keep going until the picture is shared, then stop.** "Done" is when the user can see
  the plan clearly and the vocabulary is settled enough to write a spec. Don't grill past
  that point for its own sake, and don't grill into implementation minutiae that belong in
  the spec, not here.

## Writing the glossary (CONTEXT.md)

Every time a term resolves, write it down **immediately** — don't wait for the end of the
session. The glossary is a living record that gets sharper as you talk.

**Where the file lives:**

- Default: a single `CONTEXT.md` at the repo root.
- If a `CONTEXT-MAP.md` exists at the root, the repo is multi-context. Read it, find the
  context this change belongs to, and write to *that* context's `CONTEXT.md` instead of
  the root.
- Create the file lazily — only when the first term actually crystallizes. Don't scaffold
  an empty glossary up front. (See `assets/CONTEXT.template.md` for the starting shape.)

**What belongs in it — and what emphatically does not:**

The glossary is **pure vocabulary**. Each entry answers exactly one question: *what does
this word mean in this project?* Definitions only.

- ✅ "**Collaborator** — a person added to a trip who can edit its itinerary and expenses.
  Distinct from an invitee, who has been asked but hasn't accepted."
- ❌ "Collaborators are stored in a `trip_collaborators` join table with a role enum." —
  that's implementation. It belongs in a spec or an ADR, never the glossary.
- ❌ Anything that reads like a requirement, a plan, or a how-it-works. If you catch
  yourself describing behavior or storage, stop — you've drifted from glossary into spec.

Keep it clean and readable (alphabetical is fine). The test for every line: could this sit
in a dictionary for the project? If not, it's not a glossary entry.

## Recording decisions (ADRs)

ADRs (Architecture Decision Records) capture the *why* behind a choice, for the person who
finds it six months later and wonders what you were thinking. They're powerful precisely
because they're rare — so offer one **sparingly**.

Write an ADR only when **all three** of these hold:

1. **Hard to reverse.** Undoing it later would be genuinely costly — a data model, a
   public contract, a framework commitment. Reversible choices don't need one.
2. **Surprising without context.** Someone landing on this later would reasonably ask "why
   on earth did they do it *this* way?" The ADR answers that.
3. **The result of a real trade-off.** You weighed genuine alternatives and gave something
   up. If there was no real fork in the road, there's nothing to record.

If any of the three is missing, don't write an ADR — you'd just be asking the user to
rubber-stamp a choice that didn't need ceremony. When in doubt, ask: "This feels like a
one-way door — want me to record it as an ADR?" and let them decide.

**Where and how:**

- Files live under `docs/adr/`, created lazily.
- Name them `NNNN-short-slug.md` — find the highest existing number and increment,
  zero-padded to four digits (`0001-…`, `0002-…`). The first one is `0001`.
- Use `assets/adr-template.md` as the shape: Title, Status, Context, Decision,
  Consequences.

## Knowing when you're done

You're finished when the user can describe the change clearly, the vocabulary is settled,
and the one-way decisions are recorded. At that point:

1. Give a short recap of what got sharpened — the key terms now in the glossary and any
   ADRs written.
2. Point at the next step: the settled understanding is ready for `to-spec` to turn into a
   spec, without re-interviewing.

Don't overrun the finish line. The goal was shared understanding and a paper trail, not an
exhaustive spec — that comes next in the chain.

## It's working if

- You ask one question at a time and wait, rather than dumping a questionnaire.
- Terms hit `CONTEXT.md` the moment they resolve, written in the project's own words.
- You reach into the codebase to answer your own questions wherever you can.
- ADRs stay rare — the user is never asked to rubber-stamp a reversible choice.

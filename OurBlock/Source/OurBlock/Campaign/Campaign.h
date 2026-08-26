// This folder has no source of its own. Campaign.h and every .cpp beside it are
// one-line shims that #include the real file in ../../../../cpp/campaign - campaign.hpp
// and the six .cpp files, but not campaign_test.cpp, which stays where its own build
// (cpp/campaign/README.md) runs it. See that README for what this module wraps and why
// (ADR 0014: port, don't bind).
//
// These shims live inside OurBlock's own module rather than a separate one on purpose.
// Unreal Editor builds are modular - every module is its own DLL - and cpp/campaign was
// deliberately written with zero UE dependency, so it has no dllexport/dllimport
// decoration on anything. A separate module would need that decoration added just to
// cross the DLL boundary, which means touching the one file this project keeps as its
// tested, standalone-buildable specification purely to satisfy a Windows linking
// mechanism it was never written to know about. Compiling it as part of the same DLL
// that calls it (OurBlock's) sidesteps the problem entirely: there is no boundary to
// cross, so nothing needs exporting.
//
// Quoted includes resolve relative to the file that contains them, not the file that
// pulled it in - so apply.cpp's own #include "campaign.hpp" still finds
// cpp/campaign/campaign.hpp correctly even though it arrives here via a shim in a
// different directory. This is standard behaviour, not a trick specific to this
// compiler.
#pragma once

#include "../../../../cpp/campaign/campaign.hpp"

/*
* ui_helpers.h
* Shared UI constants and inline helper functions for the Presentation layer.
*/

#pragma once

#include <imgui.h>
#include <cstdio>
#include <cstring>
#include <cctype>

// Mutable theme colors (updated by setThemeColors)

inline ImVec4 C_BG_BASE;
inline ImVec4 C_BG_PANEL;
inline ImVec4 C_BG_ELEVATED;
inline ImVec4 C_BG_SELECTED;
inline ImVec4 C_ACCENT;
inline ImVec4 C_ACCENT_HOVER;
inline ImVec4 C_ACCENT_DIM;
inline ImVec4 C_TEXT_PRI;
inline ImVec4 C_TEXT_SEC;
inline ImVec4 C_TEXT_DIS;
inline ImVec4 C_BORDER;
inline ImVec4 C_DANGER;
inline ImVec4 C_DANGER_HOVER;
inline ImVec4 C_SUCCESS;

inline const float SIDEBAR_W = 220.f;
inline const float TOPBAR_H = 56.f;

inline bool g_darkMode = true;

// Call this whenever the theme changes — updates all C_* tokens.
inline void setThemeColors(bool dark) {
    g_darkMode = dark;
    if (dark) {
        C_BG_BASE = { 0.039f, 0.039f, 0.039f, 1.f }; // #0A0A0A
        C_BG_PANEL = { 0.071f, 0.071f, 0.075f, 1.f }; // #121213
        C_BG_ELEVATED = { 0.110f, 0.110f, 0.118f, 1.f }; // #1C1C1E
        C_BG_SELECTED = { 0.071f, 0.173f, 0.310f, 1.f }; // #124F7D
        C_ACCENT = { 0.000f, 0.478f, 1.000f, 1.f }; // #007AFF
        C_ACCENT_HOVER = { 0.102f, 0.549f, 1.000f, 1.f }; // #1A8CFF
        C_ACCENT_DIM = { 0.000f, 0.478f, 1.000f, 0.18f };
        C_TEXT_PRI = { 1.000f, 1.000f, 1.000f, 1.f }; // #FFFFFF
        C_TEXT_SEC = { 0.557f, 0.557f, 0.576f, 1.f }; // #8E8E93
        C_TEXT_DIS = { 0.310f, 0.310f, 0.322f, 1.f }; // #4F4F52
        C_BORDER = { 0.173f, 0.173f, 0.180f, 1.f }; // #2C2C2E
        C_DANGER = { 1.000f, 0.231f, 0.188f, 1.f }; // #FF3B30
        C_DANGER_HOVER = { 1.000f, 0.380f, 0.349f, 1.f };
        C_SUCCESS = { 0.196f, 0.843f, 0.294f, 1.f }; // #32D74B
    }
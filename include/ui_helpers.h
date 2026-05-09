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
inline const float TOPBAR_H  =  56.f;

inline bool g_darkMode = true;

// Call this whenever the theme changes — updates all C_* tokens.
inline void setThemeColors(bool dark) {
    g_darkMode = dark;
    if (dark) {
        C_BG_BASE      = {0.039f, 0.039f, 0.039f, 1.f}; // #0A0A0A
        C_BG_PANEL     = {0.071f, 0.071f, 0.075f, 1.f}; // #121213
        C_BG_ELEVATED  = {0.110f, 0.110f, 0.118f, 1.f}; // #1C1C1E
        C_BG_SELECTED  = {0.071f, 0.173f, 0.310f, 1.f}; // #124F7D
        C_ACCENT       = {0.000f, 0.478f, 1.000f, 1.f}; // #007AFF
        C_ACCENT_HOVER = {0.102f, 0.549f, 1.000f, 1.f}; // #1A8CFF
        C_ACCENT_DIM   = {0.000f, 0.478f, 1.000f, 0.18f};
        C_TEXT_PRI     = {1.000f, 1.000f, 1.000f, 1.f}; // #FFFFFF
        C_TEXT_SEC     = {0.557f, 0.557f, 0.576f, 1.f}; // #8E8E93
        C_TEXT_DIS     = {0.310f, 0.310f, 0.322f, 1.f}; // #4F4F52
        C_BORDER       = {0.173f, 0.173f, 0.180f, 1.f}; // #2C2C2E
        C_DANGER       = {1.000f, 0.231f, 0.188f, 1.f}; // #FF3B30
        C_DANGER_HOVER = {1.000f, 0.380f, 0.349f, 1.f};
        C_SUCCESS      = {0.196f, 0.843f, 0.294f, 1.f}; // #32D74B
    } 
    else {
        C_BG_BASE      = {0.949f, 0.949f, 0.957f, 1.f}; // #F2F2F4
        C_BG_PANEL     = {1.000f, 1.000f, 1.000f, 1.f}; // #FFFFFF
        C_BG_ELEVATED  = {0.898f, 0.898f, 0.918f, 1.f}; // #E5E5EA
        C_BG_SELECTED  = {0.816f, 0.906f, 1.000f, 1.f}; // #D0E7FF
        C_ACCENT       = {0.000f, 0.443f, 0.890f, 1.f}; // #0071E3
        C_ACCENT_HOVER = {0.000f, 0.365f, 0.737f, 1.f}; // #005DBC
        C_ACCENT_DIM   = {0.000f, 0.443f, 0.890f, 0.12f};
        C_TEXT_PRI     = {0.114f, 0.114f, 0.122f, 1.f}; // #1D1D1F
        C_TEXT_SEC     = {0.431f, 0.431f, 0.451f, 1.f}; // #6E6E73
        C_TEXT_DIS     = {0.682f, 0.682f, 0.698f, 1.f}; // #AEAEB2
        C_BORDER       = {0.820f, 0.820f, 0.843f, 1.f}; // #D2D2D7
        C_DANGER       = {0.800f, 0.067f, 0.035f, 1.f}; // #CC1109
        C_DANGER_HOVER = {0.650f, 0.050f, 0.025f, 1.f};
        C_SUCCESS      = {0.098f, 0.651f, 0.173f, 1.f}; // #19A62C
    }
}

// ─── Avatar Palette ───────────────────────────────────────────────────────────

inline const ImVec4 AVATAR_COLORS[] = {
    {0.000f, 0.478f, 1.000f, 1.f},
    {1.000f, 0.231f, 0.188f, 1.f},
    {0.196f, 0.843f, 0.294f, 1.f},
    {0.694f, 0.322f, 0.871f, 1.f},
    {1.000f, 0.584f, 0.000f, 1.f},
    {0.196f, 0.784f, 0.784f, 1.f},
    {1.000f, 0.176f, 0.333f, 1.f},
    {0.353f, 0.784f, 0.980f, 1.f},
};
inline constexpr int AVATAR_COLOR_COUNT = 8;

// ─── Helpers ──────────────────────────────────────────────────────────────────

inline ImU32 toU32(ImVec4 c) { return ImGui::ColorConvertFloat4ToU32(c); }

// Row highlight colors that adapt to current theme
inline ImU32 rowSelectedBg() {
    return g_darkMode ? IM_COL32(28, 50, 85, 255) : toU32(C_BG_SELECTED);
}
inline ImU32 rowHoveredBg() {
    return g_darkMode ? IM_COL32(255, 255, 255, 8) : IM_COL32(0, 0, 0, 9);
}

inline void hDivider() {
    ImVec2 p = ImGui::GetCursorScreenPos();
    float  w = ImGui::GetContentRegionAvail().x;
    ImGui::GetWindowDrawList()->AddLine(p, {p.x + w, p.y}, toU32(C_BORDER));
    ImGui::Dummy({0.f, 1.f});
}

inline void sectionTitle(const char* title) {
    ImGui::TextColored(C_TEXT_SEC, "%s", title);
    ImGui::Dummy({0.f, 4.f});
}

inline bool accentBtn(const char* label, ImVec2 sz = {0, 0}) {
    ImGui::PushStyleColor(ImGuiCol_Button,        C_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C_ACCENT_HOVER);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  {C_ACCENT.x * 0.8f, C_ACCENT.y * 0.8f, C_ACCENT.z * 0.8f, 1.f});
    ImGui::PushStyleColor(ImGuiCol_Text,          {1.f, 1.f, 1.f, 1.f});
    bool r = ImGui::Button(label, sz);
    ImGui::PopStyleColor(4);
    ImVec2 mn = ImGui::GetItemRectMin(), mx = ImGui::GetItemRectMax();
    float  my = mn.y + (mx.y - mn.y) * 0.45f;
    ImU32  sh = ImGui::ColorConvertFloat4ToU32({1.f, 1.f, 1.f, 0.10f});
    ImU32  tr = ImGui::ColorConvertFloat4ToU32({0.f, 0.f, 0.f, 0.00f});
    ImGui::GetWindowDrawList()->AddRectFilledMultiColor(mn, {mx.x, my}, sh, sh, tr, tr);
    return r;
}

inline bool secondaryBtn(const char* label, ImVec2 sz = {0, 0}) {
    ImGui::PushStyleColor(ImGuiCol_Button,        C_BG_PANEL);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C_BG_ELEVATED);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  C_BG_ELEVATED);
    ImGui::PushStyleColor(ImGuiCol_Text,          C_TEXT_PRI);
    ImGui::PushStyleColor(ImGuiCol_Border,        C_BORDER);
    bool r = ImGui::Button(label, sz);
    ImGui::PopStyleColor(5);
    return r;
}

inline bool dangerBtn(const char* label, ImVec2 sz = {0, 0}) {
    ImGui::PushStyleColor(ImGuiCol_Button,        {C_DANGER.x, C_DANGER.y, C_DANGER.z, 0.12f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {C_DANGER_HOVER.x, C_DANGER_HOVER.y, C_DANGER_HOVER.z, 0.22f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  {C_DANGER.x, C_DANGER.y, C_DANGER.z, 0.35f});
    ImGui::PushStyleColor(ImGuiCol_Text,          C_DANGER);
    bool r = ImGui::Button(label, sz);
    ImGui::PopStyleColor(4);
    return r;
}

inline void groupBadge(const char* text) {
    ImVec2 tSz = ImGui::CalcTextSize(text);
    float  px  = 8.f, py = 3.f;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 end = {pos.x + tSz.x + px * 2.f, pos.y + tSz.y + py * 2.f};
    ImGui::GetWindowDrawList()->AddRectFilled(pos, end, toU32(C_ACCENT_DIM), (end.y - pos.y) * 0.5f);
    ImGui::SetCursorScreenPos({pos.x + px, pos.y + py});
    ImGui::TextColored(C_ACCENT, "%s", text);
}

inline void statCard(const char* id, const char* label,
                     const char* value, ImVec4 barColor, float width) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_PANEL);
    ImGui::BeginChild(id, {width, 92.f}, true, ImGuiWindowFlags_NoScrollbar);
    ImVec2 wp = ImGui::GetWindowPos(), ws = ImGui::GetWindowSize();
    ImGui::GetWindowDrawList()->AddRectFilled(wp, {wp.x + 4.f, wp.y + ws.y}, toU32(barColor));
    ImGui::SetCursorPos({16.f, 12.f});
    ImGui::TextColored(C_TEXT_SEC, "%s", label);
    ImGui::SetCursorPos({16.f, 34.f});
    ImGui::SetWindowFontScale(1.6f);
    ImGui::TextColored(barColor, "%s", value);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// Draw a circular avatar with initials. Advances cursor by size x size.
inline void drawAvatar(const char* name, float size) {
    float  radius = size * 0.5f;
    ImVec2 pos    = ImGui::GetCursorScreenPos();
    ImVec2 center = {pos.x + radius, pos.y + radius};
    int    ci     = name[0] ? (unsigned char)name[0] % AVATAR_COLOR_COUNT : 0;

    ImGui::GetWindowDrawList()->AddCircleFilled(center, radius, toU32(AVATAR_COLORS[ci]), 64);

    char ini[3] = {};
    ini[0] = name[0] ? (char)toupper((unsigned char)name[0]) : '?';
    for (int i = 1; name[i]; i++)
        if (name[i - 1] == ' ' && name[i] != ' ') {
            ini[1] = (char)toupper((unsigned char)name[i]);
            break;
        }

    ImVec2 tSz = ImGui::CalcTextSize(ini);
    ImGui::GetWindowDrawList()->AddText(
        {center.x - tSz.x * 0.5f, center.y - tSz.y * 0.5f},
        IM_COL32(255, 255, 255, 255), ini);

    ImGui::Dummy({size, size});
}

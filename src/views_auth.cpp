/*
 * views_auth.cpp
 * Presentation Layer — Login and Sign Up screens.
 * Implements renderAuthView declared in presentation.h.
 */

#include "presentation.h"
#include "auth.h"
#include "ui_helpers.h"
#include "logo.h"

#include <imgui.h>
#include <cstring>
#include <cstdio>


// Clickable link text styled in accent color
static bool linkText(const char* label) {
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{0,0,0,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0,0,0,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{0,0,0,0});
    ImGui::PushStyleColor(ImGuiCol_Text,          C_ACCENT);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0.f, 0.f});
    bool clicked = ImGui::Button(label);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    // Underline on hover
    if (ImGui::IsItemHovered()) {
        ImVec2 mn = ImGui::GetItemRectMin();
        ImVec2 mx = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddLine(
            {mn.x, mx.y}, {mx.x, mx.y}, toU32(C_ACCENT), 1.f);
    }
    return clicked;
}

// Error banner helper
static void errorBanner(const char* msg) {
    if (!msg) return;
    ImVec2 ep = ImGui::GetCursorScreenPos();
    float  ew = ImGui::GetContentRegionAvail().x;
    ImGui::GetWindowDrawList()->AddRectFilled(
        ep, {ep.x + ew, ep.y + 38.f},
        toU32({C_DANGER.x, C_DANGER.y, C_DANGER.z, 0.14f}), 6.f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
    ImGui::TextColored(C_DANGER, "%s", msg);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
}

static void fieldLabel(const char* label, bool required = false) {
    ImGui::TextColored(C_TEXT_SEC, "%s", label);
    if (required) {
        ImGui::SameLine(0, 4.f);
        ImGui::TextColored(C_DANGER, "*");
    }
    ImGui::Dummy({0.f, 4.f});
}

static void pushInputStyle() {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, C_BG_ELEVATED);
    ImGui::PushStyleColor(ImGuiCol_Border,  C_BORDER);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {12.f, 10.f});
}
static void popInputStyle() {
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

void renderAuthView(AppState& st, ContactStore& store, UserStore& users) {
    float winW = ImGui::GetContentRegionAvail().x;
    float winH = ImGui::GetContentRegionAvail().y;

    const float LEFT_W = winW * 0.44f;
    const float RIGHT_W = winW - LEFT_W;

    // ── Left panel — branding ─────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_PANEL);
    ImGui::BeginChild("##authLeft", {LEFT_W, winH}, false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImDrawList* ldl = ImGui::GetWindowDrawList();
    ImVec2 lwp = ImGui::GetWindowPos();

    // Accent stripe at top
    ldl->AddRectFilled(lwp, {lwp.x + LEFT_W, lwp.y + 3.f}, toU32(C_ACCENT));

    // Subtle right border
    ldl->AddLine({lwp.x + LEFT_W - 1.f, lwp.y},
                 {lwp.x + LEFT_W - 1.f, lwp.y + winH}, toU32(C_BORDER));

    float fs = ImGui::GetFontSize();

    // Logo: use most of the left panel width so it scales up properly
    float logoMaxW = LEFT_W * 0.82f;
    float logoMaxH = winH * 0.22f;   // allow up to 22% of height

    // Start at ~10% from top
    float startY = winH * 0.10f;
    ImGui::SetCursorPosY(startY);

    // Logo
    {
        unsigned int tex = st.darkMode ? g_logoDarkTexture : g_logoTexture;
        int srcW = st.darkMode ? g_logoDarkW : g_logoW;
        int srcH = st.darkMode ? g_logoDarkH : g_logoH;
        if (tex && srcW > 0 && srcH > 0) {
            float lw = logoMaxW;
            float lh = lw * (float)srcH / (float)srcW;
            if (lh > logoMaxH) { lh = logoMaxH; lw = lh * (float)srcW / (float)srcH; }
            ImGui::SetCursorPosX((LEFT_W - lw) * 0.5f);
            ImGui::Image((ImTextureID)(intptr_t)tex, {lw, lh});
        } else {
            ImGui::SetWindowFontScale(2.5f);
            float tw = ImGui::CalcTextSize("StructForce").x;
            ImGui::SetCursorPosX((LEFT_W - tw) * 0.5f);
            ImGui::TextColored(C_ACCENT, "StructForce");
            ImGui::SetWindowFontScale(1.f);
        }
    }

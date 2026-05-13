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

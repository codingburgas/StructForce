/*
 * presentation.cpp
 * Presentation Layer — StructForce Contact Management System
 *
 * Theme setup, AppState lifecycle, Notification toast, renderApp.
 * Supports light and dark themes via applyTheme(bool dark).
 */

#include "presentation.h"
#include "auth.h"
#include "ui_helpers.h"
#include "logo.h"
#include "logic.h"

#include <imgui.h>
#include <cstring>
#include <cstdio>

// ─── Theme ────────────────────────────────────────────────────────────────────

void applyTheme(bool dark) {
    setThemeColors(dark);

    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowPadding     = {16.f, 16.f};
    s.FramePadding      = { 8.f,  6.f};
    s.ItemSpacing       = { 8.f,  7.f};
    s.ItemInnerSpacing  = { 6.f,  4.f};
    s.IndentSpacing     = 20.f;
    s.ScrollbarSize     = 8.f;
    s.GrabMinSize       = 8.f;
    s.WindowRounding    = 0.f;
    s.ChildRounding     = 8.f;
    s.FrameRounding     = 6.f;
    s.PopupRounding     = 8.f;
    s.ScrollbarRounding = 4.f;
    s.GrabRounding      = 4.f;
    s.TabRounding       = 6.f;
    s.WindowBorderSize  = 0.f;
    s.ChildBorderSize   = 1.f;
    s.PopupBorderSize   = 1.f;
    s.FrameBorderSize   = 1.f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]             = C_BG_BASE;
    c[ImGuiCol_ChildBg]              = C_BG_PANEL;
    c[ImGuiCol_PopupBg]              = C_BG_PANEL;
    c[ImGuiCol_Border]               = C_BORDER;
    c[ImGuiCol_BorderShadow]         = {0, 0, 0, 0};
    c[ImGuiCol_FrameBg]              = C_BG_ELEVATED;
    c[ImGuiCol_FrameBgHovered]       = C_BG_SELECTED;
    c[ImGuiCol_FrameBgActive]        = C_BG_SELECTED;
    c[ImGuiCol_TitleBg]              = C_BG_PANEL;
    c[ImGuiCol_TitleBgActive]        = C_BG_PANEL;
    c[ImGuiCol_TitleBgCollapsed]     = C_BG_PANEL;
    c[ImGuiCol_ScrollbarBg]          = C_BG_BASE;
    c[ImGuiCol_ScrollbarGrab]        = C_BORDER;
    c[ImGuiCol_ScrollbarGrabHovered] = C_TEXT_DIS;
    c[ImGuiCol_ScrollbarGrabActive]  = C_TEXT_SEC;
    c[ImGuiCol_Button]               = C_BG_ELEVATED;
    c[ImGuiCol_ButtonHovered]        = C_BG_SELECTED;
    c[ImGuiCol_ButtonActive]         = C_BG_ELEVATED;
    c[ImGuiCol_Header]               = C_BG_SELECTED;
    c[ImGuiCol_HeaderHovered]        = C_BG_SELECTED;
    c[ImGuiCol_HeaderActive]         = C_BG_SELECTED;
    c[ImGuiCol_Separator]            = C_BORDER;
    c[ImGuiCol_Text]                 = C_TEXT_PRI;
    c[ImGuiCol_TextDisabled]         = C_TEXT_DIS;
    c[ImGuiCol_TableHeaderBg]        = C_BG_ELEVATED;
    c[ImGuiCol_TableBorderStrong]    = C_BORDER;
    c[ImGuiCol_TableBorderLight]     = C_BORDER;
    c[ImGuiCol_TableRowBg]           = {0.f, 0.f, 0.f, 0.f};
    c[ImGuiCol_TableRowBgAlt]        = {0.f, 0.f, 0.f, 0.f};
    c[ImGuiCol_CheckMark]            = C_ACCENT;
    c[ImGuiCol_SliderGrab]           = C_ACCENT;
    c[ImGuiCol_SliderGrabActive]     = C_ACCENT_HOVER;
    c[ImGuiCol_ModalWindowDimBg]     = {0.f, 0.f, 0.f, 0.45f};
}

// ─── AppState Init ────────────────────────────────────────────────────────────

void initAppState(AppState& st) {
    st.activeView          = VIEW_CONTACTS;
    st.darkMode            = true;
    st.formName[0]         = '\0';
    st.formPhone[0]        = '\0';
    st.formEmail[0]        = '\0';
    st.formGroup[0]        = '\0';
    st.formError           = nullptr;
    st.sortField           = SORT_BY_NAME;
    st.sortOrder           = SORT_ASC;
    st.sortDirty           = true;
    st.searchQuery[0]      = '\0';
    st.selectedContactId   = -1;
    st.editingContactId    = -1;
    st.loggedInUserId   = -1;
    st.authScreen       = AUTH_LOGIN;
    st.authFirstName[0] = '\0';
    st.authLastName[0]  = '\0';
    st.authEmail[0]     = '\0';
    st.authBirthDate[0] = '\0';
    st.authPassword[0]  = '\0';
    st.authError        = nullptr;
    st.notifMessage[0]     = '\0';
    st.notifTimer          = 0.f;
    st.notifType           = NOTIF_SUCCESS;
    for (int i = 0; i < MAX_CONTACTS; i++) st.sortedIndices[i] = i;
}

void showNotification(AppState& st, const char* msg, NotificationType type) {
    strncpy(st.notifMessage, msg, sizeof(st.notifMessage) - 1);
    st.notifMessage[sizeof(st.notifMessage) - 1] = '\0';
    st.notifTimer = 3.0f;
    st.notifType  = type;
}

void invalidateSort(AppState& st) {
    st.sortDirty = true;
}

// ─── Toast Notification ───────────────────────────────────────────────────────

void renderNotification(AppState& st, float deltaTime) {
    if (st.notifTimer <= 0.f) return;
    st.notifTimer -= deltaTime;
    if (st.notifTimer < 0.f) st.notifTimer = 0.f;

    float alpha = (st.notifTimer < 0.5f) ? st.notifTimer / 0.5f : 1.f;

    ImVec4 bg = (st.notifType == NOTIF_SUCCESS)
        ? ImVec4{0.071f, 0.271f, 0.141f, 0.92f}
        : ImVec4{0.380f, 0.090f, 0.090f, 0.92f};
    bg.w *= alpha;

    const float W = 300.f, H = 44.f, M = 20.f;
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos({vp->Pos.x + vp->Size.x - W - M,
                             vp->Pos.y + vp->Size.y - H - M});
    ImGui::SetNextWindowSize({W, H});
    ImGui::SetNextWindowBgAlpha(bg.w);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);
    ImGui::PushStyleColor(ImGuiCol_Border,   {0,0,0,0});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,  {14.f, 12.f});

    ImGuiWindowFlags f =
        ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_NoResize   |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs   |
        ImGuiWindowFlags_NoNav       | ImGuiWindowFlags_NoMove     |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::Begin("##toast", nullptr, f);
    ImVec4 tc = (st.notifType == NOTIF_SUCCESS)
        ? ImVec4{C_SUCCESS.x, C_SUCCESS.y, C_SUCCESS.z, alpha}
        : ImVec4{C_DANGER.x,  C_DANGER.y,  C_DANGER.z,  alpha};
    ImGui::TextColored(tc, "%s", st.notifMessage);
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

// ─── Main Render Entry ────────────────────────────────────────────────────────

void renderApp(AppState& st, ContactStore& store, UserStore& users) {
    // Apply theme every frame so toggling takes effect immediately
    applyTheme(st.darkMode);

    if (st.loggedInUserId < 0) {
        renderAuthView(st, store, users);
        return;
    }

    // Sidebar
    renderSidebar(st, store, users);
    ImGui::SameLine(0, 0);

    ImGui::BeginGroup();

    // ── Top bar
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_PANEL);
    ImGui::BeginChild("##topbar", {0.f, TOPBAR_H}, false, ImGuiWindowFlags_NoScrollbar);

    // Bottom border on topbar
    {
        ImVec2 wp = ImGui::GetWindowPos();
        ImVec2 ws = ImGui::GetWindowSize();
        ImGui::GetWindowDrawList()->AddLine(
            {wp.x, wp.y + ws.y - 1.f},
            {wp.x + ws.x, wp.y + ws.y - 1.f},
            toU32(C_BORDER));
    }

    // Page title
    static const char* titles[] = {"Contacts", "Add Contact", "Analytics"};
    ImGui::SetCursorPos({20.f, (TOPBAR_H - ImGui::GetFontSize()) * 0.5f});
    ImGui::TextColored(C_TEXT_PRI, "%s", titles[(int)st.activeView]);

    // Right side: theme toggle + contact count
    char cb[48];
    int  cCount = countContactsRecursive(store, 0);
    snprintf(cb, sizeof(cb), "%d %s", cCount, cCount == 1 ? "contact" : "contacts");
    float countW = ImGui::CalcTextSize(cb).x;
    const char* themeLabel = st.darkMode ? "Light mode" : "Dark mode";
    float themeBtnW = ImGui::CalcTextSize(themeLabel).x + 20.f;

    // Contact count
    float rightX = ImGui::GetWindowWidth() - countW - 20.f;
    ImGui::SetCursorPos({rightX, (TOPBAR_H - ImGui::GetFontSize()) * 0.5f});
    ImGui::TextColored(C_TEXT_SEC, "%s", cb);

    // Theme toggle button
    float btnX = rightX - themeBtnW - 16.f;
    ImGui::SetCursorPos({btnX, (TOPBAR_H - 26.f) * 0.5f});
    ImGui::PushStyleColor(ImGuiCol_Button,        C_BG_ELEVATED);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, C_BG_SELECTED);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  C_BG_ELEVATED);
    ImGui::PushStyleColor(ImGuiCol_Text,          C_ACCENT);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
    if (ImGui::Button(themeLabel, {themeBtnW, 26.f}))
        st.darkMode = !st.darkMode;
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);

    ImGui::EndChild();
    ImGui::PopStyleColor();

    // Content area
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_BASE);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {20.f, 16.f});
    ImGui::BeginChild("##content", {0.f, 0.f}, false);

    switch (st.activeView) {
        case VIEW_CONTACTS:    renderContactsView(st, store); break;
        case VIEW_ADD_CONTACT: renderAddContact  (st, store); break;
        case VIEW_ANALYTICS:   renderAnalytics   (st, store); break;
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::EndGroup();

}

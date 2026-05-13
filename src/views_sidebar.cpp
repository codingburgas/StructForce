/*
 * views_sidebar.cpp
 * Presentation Layer — Sidebar navigation panel.
 * Renders logo, nav items (Contacts / Add Contact / Analytics) and user footer.
 */

#include "presentation.h"
#include "auth.h"
#include "ui_helpers.h"
#include "logo.h"

#include <imgui.h>
#include <cctype>
#include <cstdio>
#include <cstring>

void renderSidebar(AppState& st, ContactStore& store, UserStore& users) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_PANEL);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.f, 0.f});
    ImGui::BeginChild("##sidebar", {SIDEBAR_W, 0.f}, false, ImGuiWindowFlags_NoScrollbar);
    ImGui::PopStyleVar();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 wp = ImGui::GetWindowPos();
    ImVec2 ws = ImGui::GetWindowSize();

    // Right border separating sidebar from content
    dl->AddLine({wp.x + ws.x - 1.f, wp.y},
                {wp.x + ws.x - 1.f, wp.y + ws.y}, toU32(C_BORDER));

    // Accent stripe at the very top
    dl->AddRectFilled(wp, {wp.x + ws.x, wp.y + 3.f}, toU32(C_ACCENT));

    // ── Logo
    ImGui::Dummy({0.f, 10.f});
    unsigned int activeTex = st.darkMode ? g_logoDarkTexture : g_logoTexture;
    int          activeW   = st.darkMode ? g_logoDarkW       : g_logoW;
    int          activeH   = st.darkMode ? g_logoDarkH       : g_logoH;
    if (activeTex && activeW > 0 && activeH > 0) {
        float maxW = SIDEBAR_W - 20.f;
        float dispW = maxW;
        float dispH = dispW * (float)activeH / (float)activeW;
        float maxH = 120.f;
        if (dispH > maxH) { dispH = maxH; dispW = dispH * (float)activeW / (float)activeH; }
        ImGui::SetCursorPosX((SIDEBAR_W - dispW) * 0.5f);
        ImGui::Image((ImTextureID)(intptr_t)activeTex, {dispW, dispH});
    } else {
        ImGui::SetCursorPosX(20.f);
        ImGui::TextColored(C_ACCENT, "StructForce");
    }
    ImGui::Dummy({0.f, 10.f});

    hDivider();
    ImGui::Dummy({0.f, 10.f});

    // ── Navigation
    ImGui::Dummy({0.f, 4.f});

    struct NavItem { const char* label; ActiveView view; };
    static const NavItem items[] = {
        {"Contacts",    VIEW_CONTACTS},
        {"Add Contact", VIEW_ADD_CONTACT},
        {"Analytics",   VIEW_ANALYTICS},
    };

    float navFs = ImGui::GetFontSize();
    const float NAV_H = navFs * 2.8f;
    const float IC_X  = 20.f;
    const float TXT_X = 44.f;

    for (const NavItem& item : items) {
        bool active = (st.activeView == item.view);
        ImGui::SetCursorPosX(0.f);
        ImVec2 np = ImGui::GetCursorScreenPos();

        // Active: left accent bar + subtle background
        if (active) {
            dl->AddRectFilled(np, {np.x + 3.f, np.y + NAV_H}, toU32(C_ACCENT));
            dl->AddRectFilled(np, {np.x + SIDEBAR_W - 1.f, np.y + NAV_H},
                g_darkMode ? IM_COL32(0, 120, 255, 22) : IM_COL32(0, 113, 227, 14));
        }

        // InvisibleButton for click + hover detection
        char btnId[32];
        snprintf(btnId, sizeof(btnId), "##nav%d", (int)item.view);
        ImGui::InvisibleButton(btnId, {SIDEBAR_W - 1.f, NAV_H});
        if (ImGui::IsItemClicked())
            st.activeView = item.view;

        // Hover background
        if (ImGui::IsItemHovered() && !active)
            dl->AddRectFilled(np, {np.x + SIDEBAR_W - 1.f, np.y + NAV_H}, toU32(C_BG_ELEVATED));

        float icy     = np.y + NAV_H * 0.5f;
        float icx     = np.x + IC_X;
        ImU32 iconCol = active ? toU32(C_ACCENT) : toU32(C_TEXT_DIS);
        ImU32 textCol = active ? toU32(C_ACCENT) : toU32(C_TEXT_SEC);

        // Icon (drawn with ImDrawList primitives — no font dependency)
        if (item.view == VIEW_CONTACTS) {
            dl->AddCircleFilled({icx, icy - 5.f}, 4.5f, iconCol, 20);
            dl->AddRectFilled({icx - 6.f, icy + 1.f}, {icx + 6.f, icy + 8.f}, iconCol, 3.f);
        } else if (item.view == VIEW_ADD_CONTACT) {
            dl->AddLine({icx - 7.f, icy}, {icx + 7.f, icy}, iconCol, 2.f);
            dl->AddLine({icx, icy - 7.f}, {icx, icy + 7.f}, iconCol, 2.f);
        } else if (item.view == VIEW_ANALYTICS) {
            dl->AddRectFilled({icx - 7.f, icy - 1.f}, {icx - 3.f, icy + 7.f}, iconCol, 1.f);
            dl->AddRectFilled({icx - 1.f, icy - 7.f}, {icx + 3.f, icy + 7.f}, iconCol, 1.f);
            dl->AddRectFilled({icx + 5.f, icy - 4.f}, {icx + 9.f, icy + 7.f}, iconCol, 1.f);
        }

        // Label text — manual X so it always aligns with TXT_X
        float textY = np.y + (NAV_H - navFs) * 0.5f;
        dl->AddText(ImGui::GetFont(), navFs, {np.x + TXT_X, textY}, textCol, item.label);

        ImGui::Dummy({0.f, 2.f});
    }

    // ── Footer: avatar + name + email + logout
    float winH2 = ImGui::GetWindowHeight();
    ImGui::SetCursorPosY(winH2 - 104.f);
    ImVec2 fp = ImGui::GetCursorScreenPos();
    dl->AddLine(fp, {fp.x + SIDEBAR_W, fp.y}, toU32(C_BORDER));
    ImGui::Dummy({0.f, 10.f});

    // Find logged-in user
    const User* loggedUser = nullptr;
    for (int i = 0; i < users.count; i++) {
        if (users.users[i].id == st.loggedInUserId) {
            loggedUser = &users.users[i];
            break;
        }
    }

    if (loggedUser) {
        ImVec2 ap       = ImGui::GetCursorScreenPos();
        const float avR = 12.f;
        float avCX = ap.x + 14.f + avR;
        float avCY = ap.y + avR + 2.f;

        // Colored avatar circle with initials
        int ci = loggedUser->firstName[0]
                 ? (unsigned char)loggedUser->firstName[0] % AVATAR_COLOR_COUNT : 0;
        dl->AddCircleFilled({avCX, avCY}, avR, toU32(AVATAR_COLORS[ci]), 32);
        char ini[3] = {};
        ini[0] = loggedUser->firstName[0] ? (char)toupper((unsigned char)loggedUser->firstName[0]) : '?';
        ini[1] = loggedUser->lastName[0]  ? (char)toupper((unsigned char)loggedUser->lastName[0])  : '\0';
        ImVec2 iSz = ImGui::CalcTextSize(ini);
        dl->AddText({avCX - iSz.x * 0.5f, avCY - iSz.y * 0.5f},
            IM_COL32(255, 255, 255, 255), ini);

        // Full name (primary) and email (small, disabled)
        char fullName[MAX_FNAME_LEN + MAX_LNAME_LEN + 2];
        snprintf(fullName, sizeof(fullName), "%s %s",
            loggedUser->firstName, loggedUser->lastName);
        float textX  = avCX + avR + 8.f;
        float nameY  = ap.y + 1.f;
        float emailY = nameY + ImGui::GetFontSize() + 3.f;
        dl->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
            {textX, nameY}, toU32(C_TEXT_PRI), fullName);
        dl->AddText(ImGui::GetFont(), ImGui::GetFontSize() * 0.82f,
            {textX, emailY}, toU32(C_TEXT_DIS), loggedUser->email);

        ImGui::Dummy({0.f, avR * 2.f + 14.f});
    }

    // Logout — custom InvisibleButton with red-tint hover and arrow icon
    {
        ImGui::SetCursorPosX(12.f);
        ImVec2 btnPos = ImGui::GetCursorScreenPos();
        float  btnW   = SIDEBAR_W - 24.f;
        float  btnH   = 30.f;
        ImGui::InvisibleButton("##logout", {btnW, btnH});
        bool hov = ImGui::IsItemHovered();
        bool act = ImGui::IsItemActive();

        if (hov || act) {
            ImU32 bgCol = act
                ? IM_COL32((int)(C_DANGER.x*255),(int)(C_DANGER.y*255),(int)(C_DANGER.z*255), 38)
                : IM_COL32((int)(C_DANGER.x*255),(int)(C_DANGER.y*255),(int)(C_DANGER.z*255), 22);
            dl->AddRectFilled(btnPos, {btnPos.x + btnW, btnPos.y + btnH}, bgCol, 6.f);
        }

        const char* lbl = "Log out";
        ImVec2      lSz = ImGui::CalcTextSize(lbl);
        ImU32       lCol = hov ? toU32(C_DANGER) : toU32(C_TEXT_DIS);
        dl->AddText({btnPos.x + 12.f, btnPos.y + (btnH - lSz.y) * 0.5f}, lCol, lbl);

        if (hov) {
            float ax = btnPos.x + btnW - 14.f;
            float ay = btnPos.y + btnH * 0.5f;
            dl->AddLine({ax - 6.f, ay}, {ax + 5.f, ay}, toU32(C_DANGER), 1.5f);
            dl->AddLine({ax + 5.f, ay}, {ax + 1.f, ay - 4.f}, toU32(C_DANGER), 1.5f);
            dl->AddLine({ax + 5.f, ay}, {ax + 1.f, ay + 4.f}, toU32(C_DANGER), 1.5f);
        }

        if (ImGui::IsItemClicked()) {
            saveContactsForUser(st.loggedInUserId, store);
            initContactStore(store);
            st.loggedInUserId    = -1;
            st.authScreen        = AUTH_LOGIN;
            st.authError         = nullptr;
            st.authEmail[0]      = st.authPassword[0] = '\0';
            st.selectedContactId = -1;
            st.searchQuery[0]    = '\0';
            st.activeView        = VIEW_CONTACTS;
            invalidateSort(st);
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

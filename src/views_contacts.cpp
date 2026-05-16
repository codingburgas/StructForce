/*
 * views_contacts.cpp
 * Presentation Layer — Contacts grid view.
 */

#include "presentation.h"
#include "auth.h"
#include "ui_helpers.h"

#include <imgui.h>
#include <cstdio>
#include <cctype>
#include <cmath>

void renderContactsView(AppState& st, ContactStore& store) {

    if (st.sortDirty) {
        getSortedIndices(store, st.sortedIndices, st.sortField, st.sortOrder);
        st.sortDirty = false;
    }

    SearchResult visible = searchContacts(store, st.searchQuery);

    // ── Toolbar ───────────────────────────────────────────────────────────────
    float availW  = ImGui::GetContentRegionAvail().x;
    float segW    = 160.f;
    float searchW = availW - segW - 12.f;

    // Search bar — extra left padding for icon
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  {34.f, 8.f});
    ImGui::PushStyleColor(ImGuiCol_FrameBg, C_BG_ELEVATED);
    ImGui::SetNextItemWidth(searchW);
    if (ImGui::InputText("##search", st.searchQuery, sizeof(st.searchQuery)))
        st.selectedContactId = -1;
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

    {
        ImVec2 mn = ImGui::GetItemRectMin();
        ImVec2 mx = ImGui::GetItemRectMax();
        float  cy = (mn.y + mx.y) * 0.5f;
        ImDrawList* wdl = ImGui::GetWindowDrawList();

        // Magnifying glass icon — lens top-left, handle bottom-right at 45°
        ImVec2 ic  = {mn.x + 16.f, cy - 2.f};
        ImU32  ic_col = toU32(C_TEXT_SEC);
        float  ir  = 5.f;
        wdl->AddCircle(ic, ir, ic_col, 20, 1.8f);
        // Handle starts at bottom-right edge of lens and extends further down-right
        float  ha  = 3.14159f * 0.25f;   // 45 degrees
        ImVec2 hp1 = {ic.x + ir * cosf(ha),       ic.y + ir * sinf(ha)};
        ImVec2 hp2 = {hp1.x + 4.5f * cosf(ha),    hp1.y + 4.5f * sinf(ha)};
        wdl->AddLine(hp1, hp2, ic_col, 2.f);

        // Placeholder text
        if (st.searchQuery[0] == '\0' && !ImGui::IsItemActive())
            wdl->AddText({mn.x + 34.f, cy - ImGui::GetFontSize() * 0.5f},
                toU32(C_TEXT_DIS), "Search contacts...");
    }

    ImGui::SameLine(0, 12.f);

    // Segmented sort control
    {
        ImVec2 sp  = ImGui::GetCursorScreenPos();
        float  sh  = ImGui::GetFrameHeight() + 4.f;
        float  hw  = segW * 0.5f;
        ImDrawList* sdl = ImGui::GetWindowDrawList();

        // Track background
        sdl->AddRectFilled(sp, {sp.x + segW, sp.y + sh}, toU32(C_BG_ELEVATED), 8.f);

        // Active pill
        float ax = (st.sortField == SORT_BY_NAME) ? sp.x : sp.x + hw;
        sdl->AddRectFilled({ax + 2.f, sp.y + 2.f}, {ax + hw - 2.f, sp.y + sh - 2.f},
            toU32(C_ACCENT), 6.f);

        // Labels + sort arrow triangle
        struct Seg { const char* label; SortField field; };
        const Seg segs[] = {{"Name", SORT_BY_NAME}, {"Phone", SORT_BY_PHONE}};
        for (int i = 0; i < 2; i++) {
            bool   act  = (st.sortField == segs[i].field);
            ImVec2 tSz  = ImGui::CalcTextSize(segs[i].label);
            ImU32  col  = act ? IM_COL32(255,255,255,255) : toU32(C_TEXT_SEC);
            float  arW  = act ? 10.f : 0.f;   // extra space for arrow
            float  lx   = sp.x + i * hw + (hw - tSz.x - arW) * 0.5f;
            float  ly   = sp.y + (sh - tSz.y) * 0.5f;
            sdl->AddText({lx, ly}, col, segs[i].label);

            if (act) {
                float  ax  = lx + tSz.x + 4.f;
                float  cy2 = sp.y + sh * 0.5f;
                float  ts  = 3.5f;
                if (st.sortOrder == SORT_ASC) {
                    // Up triangle ▲
                    sdl->AddTriangleFilled(
                        {ax,      cy2 - ts},
                        {ax + ts, cy2 + ts},
                        {ax - ts, cy2 + ts}, col);
                } else {
                    // Down triangle ▼
                    sdl->AddTriangleFilled(
                        {ax,      cy2 + ts},
                        {ax + ts, cy2 - ts},
                        {ax - ts, cy2 - ts}, col);
                }
            }
        }

        // Click zones
        ImGui::SetCursorScreenPos(sp);
        ImGui::PushID("##seg");
        ImGui::InvisibleButton("L", {hw, sh});
        if (ImGui::IsItemClicked()) {
            if (st.sortField == SORT_BY_NAME)
                st.sortOrder = (st.sortOrder == SORT_ASC) ? SORT_DESC : SORT_ASC;
            else { st.sortField = SORT_BY_NAME; st.sortOrder = SORT_ASC; }
            st.sortDirty = true;
        }
        ImGui::SameLine(0, 0);
        ImGui::InvisibleButton("R", {hw, sh});
        if (ImGui::IsItemClicked()) {
            if (st.sortField == SORT_BY_PHONE)
                st.sortOrder = (st.sortOrder == SORT_ASC) ? SORT_DESC : SORT_ASC;
            else { st.sortField = SORT_BY_PHONE; st.sortOrder = SORT_ASC; }
            st.sortDirty = true;
        }
        ImGui::PopID();
    }

    ImGui::Dummy({0.f, 10.f});
    hDivider();
    ImGui::Dummy({0.f, 24.f});

    // ── Grid ──────────────────────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_BASE);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.f, 12.f});
    ImGui::BeginChild("##grid", {0.f, 0.f}, false);
    ImGui::PopStyleVar();

    auto emptyState = [&](const char* line1, const char* line2) {
        float cw = ImGui::GetContentRegionAvail().x;
        float ch = ImGui::GetContentRegionAvail().y;
        ImGui::SetCursorPos({(cw - ImGui::CalcTextSize(line1).x) * 0.5f, ch * 0.38f});
        ImGui::TextColored(C_TEXT_SEC, "%s", line1);
        ImGui::SetCursorPosX((cw - ImGui::CalcTextSize(line2).x) * 0.5f);
        ImGui::TextColored(C_TEXT_DIS, "%s", line2);
    };

    if (store.count == 0) {
        emptyState("No contacts yet.", "Use 'Add Contact' to get started.");
        ImGui::EndChild(); ImGui::PopStyleColor(); return;
    }
    if (visible.count == 0) {
        emptyState("No contacts found.", "Try a different search term.");
        ImGui::EndChild(); ImGui::PopStyleColor(); return;
    }

    // Build sorted + filtered display order
    int displayOrder[MAX_CONTACTS], displayCount = 0;
    bool inVisible[MAX_CONTACTS] = {};
    const Contact* contacts = getContacts(store);
    for (int i = 0; i < visible.count; i++)
        inVisible[visible.indices[i]] = true;
    for (int i = 0; i < store.count; i++) {
        int idx = st.sortedIndices[i];
        if (idx < store.count && inVisible[idx])
            displayOrder[displayCount++] = idx;
    }

    // Grid sizing
    const float CARD_GAP = 12.f;
    const float CARD_H   = 196.f;
    float gridW  = ImGui::GetContentRegionAvail().x;
    int   cols   = (int)((gridW + CARD_GAP) / (155.f + CARD_GAP));
    if (cols < 1) cols = 1;
    if (cols > 5) cols = 5;
    float cardW  = (gridW - (cols - 1) * CARD_GAP) / (float)cols;

    ImDrawList*  dl           = ImGui::GetWindowDrawList();
    bool         mouseClicked = ImGui::IsMouseClicked(0);
    ImVec2       mousePos     = ImGui::GetMousePos();
    int          deleteId     = -1;
    int          editId       = -1;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.f, CARD_GAP});

    for (int i = 0; i < displayCount; i++) {
        int col = i % cols;
        if (col > 0) ImGui::SameLine(0, CARD_GAP);

        int idx = displayOrder[i];
        if (idx >= store.count) continue;
        const Contact& c = contacts[idx];
        bool isSelected = (c.id == st.selectedContactId);

        ImVec2 cardMin = ImGui::GetCursorScreenPos();
        ImVec2 cardMax = {cardMin.x + cardW, cardMin.y + CARD_H};

        ImGui::PushID(c.id);
        ImGui::InvisibleButton("##card", {cardW, CARD_H});
        bool hovered = ImGui::IsItemHovered();
        if (ImGui::IsItemClicked())
            st.selectedContactId = isSelected ? -1 : c.id;
        ImGui::PopID();

        // ── Card background
        ImU32 cardBg = g_darkMode ? IM_COL32(18, 18, 21, 255) : IM_COL32(255, 255, 255, 255);
        dl->AddRectFilled(cardMin, cardMax, cardBg, 14.f);

        // ── Border
        ImU32 borderCol;
        float borderThk;
        if (isSelected)      { borderCol = toU32(C_ACCENT);    borderThk = 2.f; }
        else if (hovered)    { borderCol = toU32(C_TEXT_DIS);  borderThk = 1.f; }
        else                 { borderCol = toU32(C_BORDER);    borderThk = 1.f; }
        dl->AddRect(cardMin, cardMax, borderCol, 14.f, 0, borderThk);

        // ── Avatar
        const float AV_R = 30.f;
        ImVec2 ac = {cardMin.x + cardW * 0.5f, cardMin.y + 52.f};
        int ci = c.name[0] ? (unsigned char)c.name[0] % AVATAR_COLOR_COUNT : 0;
        dl->AddCircleFilled(ac, AV_R, toU32(AVATAR_COLORS[ci]), 64);

        char ini[3] = {};
        ini[0] = c.name[0] ? (char)toupper((unsigned char)c.name[0]) : '?';
        for (int k = 1; c.name[k]; k++)
            if (c.name[k-1] == ' ' && c.name[k] != ' ') {
                ini[1] = (char)toupper((unsigned char)c.name[k]);
                break;
            }
        ImVec2 iSz = ImGui::CalcTextSize(ini);
        dl->AddText({ac.x - iSz.x * 0.5f, ac.y - iSz.y * 0.5f},
            IM_COL32(255, 255, 255, 255), ini);

        // ── Name (clip if too long)
        float nameY = cardMin.y + 96.f;
        ImVec4 nameCol = isSelected ? C_ACCENT : C_TEXT_PRI;
        {
            ImVec2 nSz = ImGui::CalcTextSize(c.name);
            if (nSz.x <= cardW - 16.f) {
                dl->AddText({cardMin.x + (cardW - nSz.x) * 0.5f, nameY},
                    toU32(nameCol), c.name);
            } else {
                dl->PushClipRect({cardMin.x + 8.f, cardMin.y}, {cardMax.x - 8.f, cardMax.y}, true);
                dl->AddText({cardMin.x + 8.f, nameY}, toU32(nameCol), c.name);
                dl->PopClipRect();
            }
        }

        // ── Phone (scaled down)
        float phoneLineY = nameY + ImGui::GetFontSize() + 4.f;
        {
            float  sc  = 0.85f;
            float  fs  = ImGui::GetFontSize() * sc;
            ImVec2 phSz = ImGui::CalcTextSize(c.phone);
            float  phX = cardMin.x + (cardW - phSz.x * sc) * 0.5f;
            dl->AddText(ImGui::GetFont(), fs, {phX, phoneLineY}, toU32(C_TEXT_SEC), c.phone);
        }

        // ── Email (accent color, link style)
        if (c.email[0]) {
            float  sc  = 0.82f;
            float  fs  = ImGui::GetFontSize() * sc;
            ImVec2 emSz = ImGui::CalcTextSize(c.email);
            float  emLineY = phoneLineY + ImGui::GetFontSize() * 0.85f + 3.f;
            float  emX = cardMin.x + (cardW - emSz.x * sc) * 0.5f;
            dl->PushClipRect({cardMin.x + 6.f, cardMin.y}, {cardMax.x - 6.f, cardMax.y}, true);
            dl->AddText(ImGui::GetFont(), fs, {emX, emLineY}, toU32(C_ACCENT), c.email);
            dl->PopClipRect();
        }

        // ── Group badge (bottom center, hidden when selected to make room for Edit)
        if (c.group[0] && !isSelected) {
            float  sc  = 0.82f;
            float  fs  = ImGui::GetFontSize() * sc;
            ImVec2 gSz = ImGui::CalcTextSize(c.group);
            float  bPad = 7.f;
            float  bW  = gSz.x * sc + bPad * 2.f;
            float  bH  = gSz.y * sc + 4.f;
            float  bX  = cardMin.x + (cardW - bW) * 0.5f;
            float  bY  = cardMax.y - bH - 10.f;
            dl->AddRectFilled({bX, bY}, {bX + bW, bY + bH}, toU32(C_ACCENT_DIM), bH * 0.5f);
            dl->AddText(ImGui::GetFont(), fs, {bX + bPad, bY + 2.f}, toU32(C_ACCENT), c.group);
        }

        // ── Action buttons when selected
        if (isSelected) {
            // × delete — top-right circle
            ImVec2 dc   = {cardMax.x - 14.f, cardMin.y + 14.f};
            float  dr   = 11.f;
            float  ddx  = mousePos.x - dc.x, ddy = mousePos.y - dc.y;
            bool   dHov = (ddx*ddx + ddy*ddy) <= dr*dr;
            ImU32  dCol = dHov
                ? toU32(C_DANGER)
                : toU32({C_DANGER.x * 0.75f, C_DANGER.y * 0.75f, C_DANGER.z * 0.75f, 1.f});
            dl->AddCircleFilled(dc, dr, dCol, 32);
            ImVec2 xSz = ImGui::CalcTextSize("X");
            dl->AddText({dc.x - xSz.x * 0.5f, dc.y - xSz.y * 0.5f + 1.f},
                IM_COL32(255, 255, 255, 255), "X");
            if (dHov && mouseClicked) deleteId = c.id;

            // Edit — small pill button at bottom center
            const char* editLbl = "Edit";
            float  ebW = ImGui::CalcTextSize(editLbl).x + 20.f;
            float  ebH = 22.f;
            float  ebX = cardMin.x + (cardW - ebW) * 0.5f;
            float  ebY = cardMax.y - ebH - 8.f;
            bool   eHov = mousePos.x >= ebX && mousePos.x <= ebX + ebW &&
                          mousePos.y >= ebY && mousePos.y <= ebY + ebH;
            dl->AddRectFilled({ebX, ebY}, {ebX + ebW, ebY + ebH},
                eHov ? toU32(C_ACCENT) : toU32(C_ACCENT_DIM), ebH * 0.5f);
            ImVec2 eSz = ImGui::CalcTextSize(editLbl);
            dl->AddText({ebX + (ebW - eSz.x) * 0.5f, ebY + (ebH - eSz.y) * 0.5f},
                eHov ? IM_COL32(255,255,255,255) : toU32(C_ACCENT), editLbl);
            if (eHov && mouseClicked) editId = c.id;
        }
    }

    ImGui::PopStyleVar();

    if (deleteId >= 0) {
        deleteContactById(store, deleteId);
        saveContactsForUser(st.loggedInUserId, store);
        invalidateSort(st);
        if (st.selectedContactId == deleteId) st.selectedContactId = -1;
        showNotification(st, "Contact removed.", NOTIF_ERROR);
    }

    if (editId >= 0) {
        const Contact* cs = getContacts(store);
        for (int i = 0; i < store.count; i++) {
            if (cs[i].id == editId) {
                strncpy(st.formName,  cs[i].name,  MAX_NAME_LEN  - 1);
                strncpy(st.formPhone, cs[i].phone, MAX_PHONE_LEN - 1);
                strncpy(st.formEmail, cs[i].email, MAX_EMAIL_LEN - 1);
                strncpy(st.formGroup, cs[i].group, MAX_GROUP_LEN - 1);
                st.formName[MAX_NAME_LEN - 1]   = '\0';
                st.formPhone[MAX_PHONE_LEN - 1] = '\0';
                st.formEmail[MAX_EMAIL_LEN - 1] = '\0';
                st.formGroup[MAX_GROUP_LEN - 1] = '\0';
                st.editingContactId = editId;
                st.formError        = nullptr;
                st.activeView       = VIEW_ADD_CONTACT;
                break;
            }
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

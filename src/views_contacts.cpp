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

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

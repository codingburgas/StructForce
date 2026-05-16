/*
 * views_forms.cpp
 * Presentation Layer — Add Contact form and Analytics view.
 * Implements renderAddContact and renderAnalytics declared in presentation.h.
 */

#include "presentation.h"
#include "auth.h"
#include "ui_helpers.h"

#include <imgui.h>
#include <cstring>
#include <cstdio>

// ─── Add Contact View ─────────────────────────────────────────────────────────

void renderAddContact(AppState& st, ContactStore& store) {
    float availW = ImGui::GetContentRegionAvail().x;
    float availH = ImGui::GetContentRegionAvail().y;

    const float GAP    = 20.f;
    const float FORMW  = (availW * 0.58f < 520.f ? availW * 0.58f : 520.f);
    float       rightW = availW - FORMW - GAP;
    bool        showPreview = (rightW >= 220.f);

    // ── Left panel: form fields
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_PANEL);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {28.f, 28.f});
    ImGui::BeginChild("##addForm", {showPreview ? FORMW : availW, availH},
                      ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    ImGui::PopStyleVar();

    bool editMode = (st.editingContactId >= 0);

    // Title
    ImGui::SetWindowFontScale(1.25f);
    ImGui::TextColored(C_TEXT_PRI, editMode ? "Edit Contact" : "New Contact");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Dummy({0.f, 2.f});
    ImGui::TextColored(C_TEXT_SEC, editMode ? "Update the contact details below."
                                            : "Fill in the required fields below.");
    ImGui::Dummy({0.f, 18.f});
    hDivider();
    ImGui::Dummy({0.f, 20.f});

    // Error banner
    if (st.formError) {
        ImVec2 ep  = ImGui::GetCursorScreenPos();
        float  ew  = ImGui::GetContentRegionAvail().x;
        ImGui::GetWindowDrawList()->AddRectFilled(
            ep, {ep.x + ew, ep.y + 36.f},
            toU32({C_DANGER.x, C_DANGER.y, C_DANGER.z, 0.14f}), 6.f);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.f);
        ImGui::TextColored(C_DANGER, "%s", st.formError);
        ImGui::Dummy({0.f, 10.f});
    }

    // Full name
    bool nameErr = (st.formError != nullptr && st.formName[0] == '\0');
    ImGui::TextColored(nameErr ? C_DANGER : C_TEXT_SEC, "Full name");
    ImGui::SameLine(0, 4.f);
    ImGui::TextColored(C_DANGER, "*");
    ImGui::Dummy({0.f, 5.f});
    ImGui::SetNextItemWidth(-1.f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, C_BG_ELEVATED);
    ImGui::PushStyleColor(ImGuiCol_Border,  nameErr ? C_DANGER : C_BORDER);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {12.f, 10.f});
    ImGui::InputText("##fname", st.formName, MAX_NAME_LEN);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    ImGui::Dummy({0.f, 16.f});

    // Phone number
    bool phoneErr = (st.formError != nullptr && st.formPhone[0] == '\0');
    ImGui::TextColored(phoneErr ? C_DANGER : C_TEXT_SEC, "Phone number");
    ImGui::SameLine(0, 4.f);
    ImGui::TextColored(C_DANGER, "*");
    ImGui::Dummy({0.f, 5.f});
    ImGui::SetNextItemWidth(-1.f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, C_BG_ELEVATED);
    ImGui::PushStyleColor(ImGuiCol_Border,  phoneErr ? C_DANGER : C_BORDER);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {12.f, 10.f});
    ImGui::InputText("##fphone", st.formPhone, MAX_PHONE_LEN);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    ImGui::Dummy({0.f, 16.f});

    // Email address (optional)
    ImGui::TextColored(C_TEXT_SEC, "Email address");
    ImGui::Dummy({0.f, 5.f});
    ImGui::SetNextItemWidth(-1.f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, C_BG_ELEVATED);
    ImGui::PushStyleColor(ImGuiCol_Border,  C_BORDER);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {12.f, 10.f});
    ImGui::InputText("##femail", st.formEmail, MAX_EMAIL_LEN);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    ImGui::Dummy({0.f, 16.f});

    // Group / Category — combo only, no duplicate text field
    ImGui::TextColored(C_TEXT_SEC, "Group / Category");
    ImGui::Dummy({0.f, 5.f});
    ImGui::SetNextItemWidth(-1.f);
    const char* presets[] = {"Friends", "Family", "Work", "School", "General", "Other"};
    const char* preview   = st.formGroup[0] ? st.formGroup : "Select a group...";
    ImGui::PushStyleColor(ImGuiCol_FrameBg, C_BG_ELEVATED);
    ImGui::PushStyleColor(ImGuiCol_Border,  C_BORDER);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {12.f, 10.f});
    if (ImGui::BeginCombo("##fgcmb", preview)) {
        for (const char* p : presets) {
            bool sel = (strcmp(st.formGroup, p) == 0);
            if (ImGui::Selectable(p, sel))
                strncpy(st.formGroup, p, MAX_GROUP_LEN - 1);
            if (sel) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    // Buttons pinned to bottom of panel
    float btnAreaH = 28.f + 38.f + 12.f; // divider dummy + button + bottom dummy
    float spaceY   = availH - ImGui::GetCursorPosY() - btnAreaH - 56.f; // 56 = top padding
    if (spaceY > 0.f) ImGui::Dummy({0.f, spaceY});

    hDivider();
    ImGui::Dummy({0.f, 12.f});

    float btnW = (ImGui::GetContentRegionAvail().x - 8.f) / 2.f;
    if (secondaryBtn("Cancel", {btnW, 38.f})) {
        st.formName[0] = st.formPhone[0] = st.formEmail[0] = st.formGroup[0] = '\0';
        st.formError        = nullptr;
        st.editingContactId = -1;
        st.activeView       = VIEW_CONTACTS;
    }
    ImGui::SameLine(0, 8.f);
    const char* saveLabel = editMode ? "Save Changes" : "Add Contact";
    if (accentBtn(saveLabel, {btnW, 38.f})) {
        if (editMode) {
            st.formError = updateContactValidated(store, st.editingContactId,
                                                  st.formName, st.formPhone,
                                                  st.formEmail, st.formGroup);
            if (!st.formError) {
                saveContactsForUser(st.loggedInUserId, store);
                st.formName[0] = st.formPhone[0] = st.formEmail[0] = st.formGroup[0] = '\0';
                st.editingContactId = -1;
                invalidateSort(st);
                showNotification(st, "Contact updated.", NOTIF_SUCCESS);
                st.activeView = VIEW_CONTACTS;
            }
        } else {
            st.formError = addContactValidated(store, st.formName, st.formPhone,
                                               st.formEmail, st.formGroup);
            if (!st.formError) {
                saveContactsForUser(st.loggedInUserId, store);
                st.formName[0] = st.formPhone[0] = st.formEmail[0] = st.formGroup[0] = '\0';
                invalidateSort(st);
                showNotification(st, "Contact added successfully.", NOTIF_SUCCESS);
                st.activeView = VIEW_CONTACTS;
            }
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    if (!showPreview) return;

    ImGui::SameLine(0, GAP);

    // ── Right panel: live preview
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_PANEL);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {28.f, 28.f});
    ImGui::BeginChild("##addPreview", {rightW, availH},
                      ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
    ImGui::PopStyleVar();

    ImGui::TextColored(C_TEXT_DIS, "Preview");
    ImGui::Dummy({0.f, 28.f});

    // Large avatar centered
    const char* previewName = st.formName[0] ? st.formName : " ";
    float       avatarSz    = 88.f;
    float       innerW      = rightW - 56.f; // subtract 2x padding
    ImGui::SetCursorPosX((innerW - avatarSz) * 0.5f + 0.f);
    drawAvatar(previewName, avatarSz);

    ImGui::Dummy({0.f, 16.f});

    // Name — centered
    const char* nameStr = st.formName[0] ? st.formName : "Your Name";
    ImVec4      nameCol = st.formName[0] ? C_TEXT_PRI : C_TEXT_DIS;
    ImGui::SetWindowFontScale(1.15f);
    float nameW = ImGui::CalcTextSize(nameStr).x;
    ImGui::SetCursorPosX((innerW - nameW) * 0.5f);
    ImGui::TextColored(nameCol, "%s", nameStr);
    ImGui::SetWindowFontScale(1.0f);

    ImGui::Dummy({0.f, 5.f});

    // Phone — centered
    const char* phoneStr = st.formPhone[0] ? st.formPhone : "Phone number";
    ImVec4      phoneCol = st.formPhone[0] ? C_TEXT_SEC : C_TEXT_DIS;
    float phoneW = ImGui::CalcTextSize(phoneStr).x;
    ImGui::SetCursorPosX((innerW - phoneW) * 0.5f);
    ImGui::TextColored(phoneCol, "%s", phoneStr);

    // Email — centered, accent color
    if (st.formEmail[0]) {
        ImGui::Dummy({0.f, 4.f});
        float emailW = ImGui::CalcTextSize(st.formEmail).x;
        ImGui::SetCursorPosX((innerW - emailW) * 0.5f);
        ImGui::TextColored(C_ACCENT, "%s", st.formEmail);
    }

    ImGui::Dummy({0.f, 10.f});

    // Group badge — centered
    if (st.formGroup[0]) {
        ImVec2 tSz   = ImGui::CalcTextSize(st.formGroup);
        float  badgeW = tSz.x + 16.f;
        ImGui::SetCursorPosX((innerW - badgeW) * 0.5f);
        groupBadge(st.formGroup);
        ImGui::Dummy({0.f, tSz.y});
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// ─── Analytics View ───────────────────────────────────────────────────────────

void renderAnalytics(AppState& /*st*/, const ContactStore& store) {
    char buf[64];
    int  total = countContactsRecursive(store, 0);

    // ── Stat cards
    float gap   = 16.f;
    float cardW = (ImGui::GetContentRegionAvail().x - 2.f * gap) / 3.f;

    snprintf(buf, sizeof(buf), "%d", total);
    statCard("##sc1", "TOTAL CONTACTS", buf, C_ACCENT, cardW);
    ImGui::SameLine(0, gap);

    int groups = getGroupCount(store);
    snprintf(buf, sizeof(buf), "%d", groups);
    statCard("##sc2", "GROUPS", buf, C_ACCENT_HOVER, cardW);
    ImGui::SameLine(0, gap);

    char mostCommon[MAX_GROUP_LEN];
    getMostCommonGroup(store, mostCommon);
    statCard("##sc3", "TOP GROUP", mostCommon[0] ? mostCommon : "—", C_SUCCESS, cardW);

    ImGui::Dummy({0.f, 28.f});

    if (store.count == 0) {
        ImGui::TextColored(C_TEXT_DIS, "No contacts yet.");
        return;
    }

    // Build group data
    char gNames [MAX_CONTACTS][MAX_GROUP_LEN];
    int  gCounts[MAX_CONTACTS];
    int  gUnique = 0;
    const Contact* contacts = getContacts(store);

    for (int i = 0; i < store.count; i++) {
        bool found = false;
        for (int j = 0; j < gUnique; j++) {
            if (strcmp(gNames[j], contacts[i].group) == 0) {
                gCounts[j]++; found = true; break;
            }
        }
        if (!found && gUnique < MAX_CONTACTS) {
            strncpy(gNames[gUnique], contacts[i].group, MAX_GROUP_LEN - 1);
            gNames[gUnique][MAX_GROUP_LEN - 1] = '\0';
            gCounts[gUnique++] = 1;
        }
    }

    // ── Section header
    ImGui::TextColored(C_TEXT_PRI, "Group Breakdown");
    ImGui::Dummy({0.f, 4.f});
    hDivider();
    ImGui::Dummy({0.f, 12.f});

    // ── Full-width panel — rows with inline bar chart
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_PANEL);
    ImGui::BeginChild("##gBreak", {0.f, 0.f}, ImGuiChildFlags_Borders);

    float panelW   = ImGui::GetContentRegionAvail().x;
    float rowH     = 52.f;
    float colGroup = 180.f;
    float colCount = 60.f;
    float colPct   = 56.f;
    float colBar   = panelW - colGroup - colCount - colPct - 48.f;

    // Header row
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_ELEVATED);
    ImGui::BeginChild("##ghdr", {0.f, 32.f}, false, ImGuiWindowFlags_NoScrollbar);
    ImGui::SetCursorPos({16.f, 7.f});
    ImGui::TextColored(C_TEXT_DIS, "Group");
    ImGui::SameLine(colGroup + 16.f);
    ImGui::TextColored(C_TEXT_DIS, "Members");
    ImGui::SameLine(colGroup + colCount + 16.f);
    ImGui::TextColored(C_TEXT_DIS, "Share");
    ImGui::SameLine(colGroup + colCount + colPct + 16.f);
    ImGui::TextColored(C_TEXT_DIS, "Distribution");
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImDrawList* dl = ImGui::GetWindowDrawList();

    for (int i = 0; i < gUnique; i++) {
        float pct = total > 0 ? (float)gCounts[i] / (float)total : 0.f;

        ImVec2 rowPos = ImGui::GetCursorScreenPos();

        // Alternating subtle row bg
        if (i % 2 == 0) {
            dl->AddRectFilled(rowPos,
                {rowPos.x + panelW, rowPos.y + rowH},
                g_darkMode ? IM_COL32(255,255,255,5) : IM_COL32(0,0,0,5));
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (rowH - ImGui::GetFontSize()) * 0.5f - 2.f);

        // Group badge
        ImGui::SetCursorPosX(16.f);
        groupBadge(gNames[i]);

        // Member count
        ImGui::SameLine(colGroup + 16.f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
        ImGui::TextColored(C_TEXT_PRI, "%d", gCounts[i]);

        // Percentage text
        ImGui::SameLine(colGroup + colCount + 16.f);
        snprintf(buf, sizeof(buf), "%.0f%%", pct * 100.f);
        ImGui::TextColored(C_TEXT_SEC, "%s", buf);

        // Bar
        float barX  = colGroup + colCount + colPct + 32.f;
        float barY  = rowPos.y + rowH * 0.5f - 5.f;
        float barW  = colBar * pct;
        ImVec2 trackP0 = {rowPos.x + barX,        barY};
        ImVec2 trackP1 = {rowPos.x + barX + colBar, barY + 10.f};
        ImVec2 fillP1  = {rowPos.x + barX + barW,   barY + 10.f};

        dl->AddRectFilled(trackP0, trackP1,
            g_darkMode ? IM_COL32(255,255,255,15) : IM_COL32(0,0,0,12), 5.f);
        if (barW > 2.f) {
            ImVec4 bc = AVATAR_COLORS[i % AVATAR_COLOR_COUNT];
            dl->AddRectFilled(trackP0, fillP1, toU32(bc), 5.f);
        }

        // Advance cursor past the full row
        ImGui::SetCursorPosY(rowPos.y - ImGui::GetWindowPos().y + rowH);

        // Row divider
        if (i < gUnique - 1) {
            ImVec2 divY = ImGui::GetCursorScreenPos();
            dl->AddLine({divY.x, divY.y}, {divY.x + panelW, divY.y}, toU32(C_BORDER));
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

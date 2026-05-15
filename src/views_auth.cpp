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

    ImGui::Dummy({0.f, fs * 2.0f});

    // Tagline — two lines, centered
    {
        ImGui::SetWindowFontScale(1.65f);
        const char* t1 = "Your contacts,";
        ImGui::SetCursorPosX((LEFT_W - ImGui::CalcTextSize(t1).x) * 0.5f);
        ImGui::TextColored(C_TEXT_PRI, "%s", t1);
        const char* t2 = "organized.";
        ImGui::SetCursorPosX((LEFT_W - ImGui::CalcTextSize(t2).x) * 0.5f);
        ImGui::TextColored(C_ACCENT, "%s", t2);
        ImGui::SetWindowFontScale(1.f);
    }

    ImGui::Dummy({0.f, fs * 2.5f});

    // Feature bullets
    const char* bullets[] = {
        "Add & manage contacts easily",
        "Search, sort and filter instantly",
        "Group contacts by category",
        "Analytics at a glance",
    };
    float bx   = LEFT_W * 0.12f;
    float dotR = fs * 0.28f;
    for (const char* text : bullets) {
        ImVec2 tp = ImGui::GetCursorScreenPos();
        ldl->AddCircleFilled(
            {lwp.x + bx + dotR, tp.y + fs * 0.52f},
            dotR, toU32(C_ACCENT), 16);
        ImGui::SetCursorPosX(bx + dotR * 2.f + fs * 0.8f);
        ImGui::TextColored(C_TEXT_SEC, "%s", text);
        ImGui::Dummy({0.f, fs * 0.6f});
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 0);

    // ── Right panel — form ────────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_ChildBg, C_BG_BASE);
    ImGui::BeginChild("##authRight", {RIGHT_W, winH}, false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Form sizing — fixed pixel width, centered
    const float FORM_W   = (RIGHT_W - 80.f < 340.f) ? RIGHT_W - 80.f : 340.f;
    const float FORM_PAD = (RIGHT_W - FORM_W) * 0.5f;

    // Vertical centering: sit slightly above center
    float formH = (st.authScreen == AUTH_LOGIN) ? winH * 0.44f : winH * 0.66f;
    float formY = (winH - formH) * 0.38f;
    if (formY < 24.f) formY = 24.f;
    ImGui::SetCursorPos({FORM_PAD, formY});

    ImGui::BeginGroup();
    float innerW = FORM_W;

     if (st.authScreen == AUTH_LOGIN) {
        // Title
        ImGui::SetWindowFontScale(1.5f);
        ImGui::TextColored(C_TEXT_PRI, "Welcome back");
        ImGui::SetWindowFontScale(1.f);
        ImGui::Dummy({0.f, 2.f});
        ImGui::TextColored(C_TEXT_SEC, "Sign in to your account");
        ImGui::Dummy({0.f, 24.f});

        // Error
        errorBanner(st.authError);

        // Email
        fieldLabel("Email", true);
        ImGui::SetNextItemWidth(innerW);
        pushInputStyle();
        ImGui::InputText("##lemail", st.authEmail, MAX_UMAIL_LEN);
        popInputStyle();
        ImGui::Dummy({0.f, 12.f});

        // Password
        fieldLabel("Password", true);
        ImGui::SetNextItemWidth(innerW);
        pushInputStyle();
        ImGui::InputText("##lpass", st.authPassword, MAX_PASS_LEN,
                         ImGuiInputTextFlags_Password);
        popInputStyle();
        ImGui::Dummy({0.f, 20.f});

        // Sign In button
        if (accentBtn("Sign In", {innerW, 40.f})) {
            const User* u = loginUser(users, st.authEmail, st.authPassword);
            if (u) {
                st.loggedInUserId    = u->id;
                st.authError         = nullptr;
                loadContactsForUser(u->id, store);
                invalidateSort(st);
                st.activeView        = VIEW_CONTACTS;
                st.authPassword[0]   = '\0';
                st.selectedContactId = -1;
                st.searchQuery[0]    = '\0';
            } else {
                st.authError = "Invalid email or password.";
            }
        }

        ImGui::Dummy({0.f, 20.f});
        hDivider();
        ImGui::Dummy({0.f, 14.f});

        // Link
        const char* q  = "Don't have an account? ";
        const char* lk = "Create one";
        ImGui::TextColored(C_TEXT_SEC, "%s", q);
        ImGui::SameLine(0, 0);
        if (linkText(lk)) { st.authScreen = AUTH_SIGNUP; st.authError = nullptr; }

    } else {
        // Title
        ImGui::SetWindowFontScale(1.5f);
        ImGui::TextColored(C_TEXT_PRI, "Create your account");
        ImGui::SetWindowFontScale(1.f);
        ImGui::Dummy({0.f, 2.f});
        ImGui::TextColored(C_TEXT_SEC, "Join StructForce today");
        ImGui::Dummy({0.f, 24.f});

        // Error
        errorBanner(st.authError);

        // First + Last name
        float halfW = (innerW - 8.f) * 0.5f;
        ImGui::TextColored(C_TEXT_SEC, "First name");
        ImGui::SameLine(halfW + 8.f);
        ImGui::TextColored(C_TEXT_SEC, "Last name");
        ImGui::Dummy({0.f, 4.f});
        pushInputStyle();
        ImGui::SetNextItemWidth(halfW);
        ImGui::InputText("##sfname", st.authFirstName, MAX_FNAME_LEN);
        ImGui::SameLine(0, 8.f);
        ImGui::SetNextItemWidth(halfW);
        ImGui::InputText("##slname", st.authLastName, MAX_LNAME_LEN);
        popInputStyle();
        ImGui::Dummy({0.f, 12.f});

        // Email
        fieldLabel("Email address", true);
        ImGui::SetNextItemWidth(innerW);
        pushInputStyle();
        ImGui::InputText("##semail", st.authEmail, MAX_UMAIL_LEN);
        popInputStyle();
        ImGui::Dummy({0.f, 12.f});

        // Date of birth
        fieldLabel("Date of birth");
        ImGui::SetNextItemWidth(innerW);
        pushInputStyle();
        ImGui::InputText("##sbdate", st.authBirthDate, MAX_BDATE_LEN);
        if (st.authBirthDate[0] == '\0' && !ImGui::IsItemActive()) {
            ImVec2 mn = ImGui::GetItemRectMin();
            ImVec2 mx = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddText(
                {mn.x + 12.f, mn.y + (mx.y - mn.y - ImGui::GetFontSize()) * 0.5f},
                toU32(C_TEXT_DIS), "DD.MM.YYYY");
        }
        popInputStyle();
        ImGui::Dummy({0.f, 12.f});

        // Password
        fieldLabel("Password", true);
        ImGui::SetNextItemWidth(innerW);
        pushInputStyle();
                ImGui::InputText("##spass", st.authPassword, MAX_PASS_LEN,
                         ImGuiInputTextFlags_Password);
        popInputStyle();
        ImGui::Dummy({0.f, 4.f});
        ImGui::TextColored(C_TEXT_DIS, "  At least 6 characters");
        ImGui::Dummy({0.f, 16.f});

        // Create Account button
        if (accentBtn("Create Account", {innerW, 40.f})) {
            const char* err = registerUser(users,
                st.authFirstName, st.authLastName,
                st.authEmail, st.authBirthDate, st.authPassword);
            if (err) {
                st.authError = err;
            } else {
                const User* u = loginUser(users, st.authEmail, st.authPassword);
                if (u) {
                    st.loggedInUserId    = u->id;
                    st.authError         = nullptr;
                    initContactStore(store);
                    invalidateSort(st);
                    st.activeView        = VIEW_CONTACTS;
                    st.selectedContactId = -1;
                    st.searchQuery[0]    = '\0';
                    st.authFirstName[0]  = st.authLastName[0] = '\0';
                    st.authEmail[0]      = st.authBirthDate[0] = '\0';
                    st.authPassword[0]   = '\0';
                }
            }
        }

        ImGui::Dummy({0.f, 16.f});
        hDivider();
        ImGui::Dummy({0.f, 12.f});

        // Link
        const char* q  = "Already have an account? ";
        const char* lk = "Sign in";
        ImGui::TextColored(C_TEXT_SEC, "%s", q);
        ImGui::SameLine(0, 0);
        if (linkText(lk)) { st.authScreen = AUTH_LOGIN; st.authError = nullptr; }
    }

    ImGui::EndGroup();
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

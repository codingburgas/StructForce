/*
 * presentation.h
 * Presentation Layer — StructForce Contact Management System
 *
 * Declares AppState and all render functions.
 * Presentation communicates ONLY with the Logic layer (logic.h).
 * It never includes data.h directly.
 *
 * Layout:  [Sidebar 200px] | [Content area]
 * Views:   Contacts · Add Contact · Analytics
 */

#pragma once

#include "logic.h"  // pulls in data.h transitively
#include "auth.h"

// ─── Enumerations ─────────────────────────────────────────────────────────────

enum AuthView {
    AUTH_LOGIN  = 0,
    AUTH_SIGNUP = 1
};

enum ActiveView {
    VIEW_CONTACTS    = 0,
    VIEW_ADD_CONTACT = 1,
    VIEW_ANALYTICS   = 2
};

enum NotificationType {
    NOTIF_SUCCESS = 0,
    NOTIF_ERROR   = 1
};

// ─── AppState ─────────────────────────────────────────────────────────────────

/*
 * AppState — all ephemeral UI state.
 * No classes, no heap allocation. Passed by reference to every render function.
 * Lives as a single instance in main().
 */
struct AppState {

    // Navigation
    ActiveView activeView;

    // Theme
    bool        darkMode;

    // Add Contact form buffers
    char        formName [MAX_NAME_LEN];
    char        formPhone[MAX_PHONE_LEN];
    char        formEmail[MAX_EMAIL_LEN];
    char        formGroup[MAX_GROUP_LEN];
    const char* formError;       // nullptr = no error

    // Sort state (Contacts view)
    SortField   sortField;
    SortOrder   sortOrder;
    int         sortedIndices[MAX_CONTACTS];
    bool        sortDirty;

    // Live search query (Contacts view)
    char        searchQuery[MAX_NAME_LEN + MAX_PHONE_LEN];

    // Selected row (Contacts view table)
    int         selectedContactId;

    // Edit mode (-1 = adding new, >=0 = editing this contact id)
    int         editingContactId;

    // Auth state
    int         loggedInUserId;
    AuthView    authScreen;
    char        authFirstName[MAX_FNAME_LEN];
    char        authLastName [MAX_LNAME_LEN];
    char        authEmail    [MAX_UMAIL_LEN];
    char        authBirthDate[MAX_BDATE_LEN];
    char        authPassword [MAX_PASS_LEN];
    const char* authError;

    // Toast notification
    char             notifMessage[256];
    float            notifTimer;
    NotificationType notifType;
};

// ─── Lifecycle ────────────────────────────────────────────────────────────────

void initAppState(AppState& state);
void applyTheme(bool dark);

// ─── State Helpers ────────────────────────────────────────────────────────────

void showNotification(AppState& state, const char* message, NotificationType type);
void invalidateSort(AppState& state);

// ─── Render Entry Point ───────────────────────────────────────────────────────

void renderApp(AppState& state, ContactStore& store, UserStore& users);

// ─── View Renderers ───────────────────────────────────────────────────────────

void renderSidebar     (AppState& state, ContactStore& store, UserStore& users);
void renderContactsView(AppState& state, ContactStore& store);
void renderAddContact  (AppState& state, ContactStore& store);
void renderAnalytics   (AppState& state,       const ContactStore& store);
void renderNotification(AppState& state, float deltaTime);
void renderAuthView    (AppState& state, ContactStore& store, UserStore& users);

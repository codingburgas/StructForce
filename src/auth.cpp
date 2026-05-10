// user account management and binary file persistence
 

#include "auth.h"
#include <cstdio>
#include <cstring>
#include <cctype>

// djb2 hash
unsigned int hashPassword(const char* pw) {
    unsigned int h = 5381;
    for (int i = 0; pw[i]; i++)
        h = ((h << 5) + h) + (unsigned char)pw[i];
    return h;
}

void initUserStore(UserStore& us) {
    us.count  = 0;
    us.nextId = 1;
    for (int i = 0; i < MAX_USERS; i++) us.users[i] = {};
}

bool saveUserStore(const UserStore& us) {
    FILE* f = fopen("users.dat", "wb");
    if (!f) return false;
    fwrite(&us, sizeof(UserStore), 1, f);
    fclose(f);
    return true;
}

bool loadUserStore(UserStore& us) {
    FILE* f = fopen("users.dat", "rb");
    if (!f) { initUserStore(us); return false; }
    bool ok = (fread(&us, sizeof(UserStore), 1, f) == 1);
    fclose(f);
    if (!ok) initUserStore(us);
    return ok;
}

static bool emailExists(const UserStore& us, const char* email) {
    for (int i = 0; i < us.count; i++)
        if (strcmp(us.users[i].email, email) == 0) return true;
    return false;
}

const char* registerUser(UserStore& us,
                         const char* firstName, const char* lastName,
                         const char* email,     const char* birthDate,
                         const char* password) {
    if (!firstName || firstName[0] == '\0') return "First name is required.";
    if (!lastName  || lastName[0]  == '\0') return "Last name is required.";
    if (!email     || email[0]     == '\0') return "Email is required.";
    if (!password  || password[0]  == '\0') return "Password is required.";
    if ((int)strlen(password) < 6)          return "Password must be at least 6 characters.";
    if (us.count >= MAX_USERS)              return "User limit reached.";
    if (emailExists(us, email))             return "An account with this email already exists.";

    User& u = us.users[us.count];
    u.id = us.nextId++;
    strncpy(u.firstName, firstName,             MAX_FNAME_LEN - 1); u.firstName[MAX_FNAME_LEN-1] = '\0';
    strncpy(u.lastName,  lastName,              MAX_LNAME_LEN - 1); u.lastName [MAX_LNAME_LEN-1] = '\0';
    strncpy(u.email,     email,                 MAX_UMAIL_LEN - 1); u.email    [MAX_UMAIL_LEN-1] = '\0';
    strncpy(u.birthDate, birthDate ? birthDate : "", MAX_BDATE_LEN - 1);
    u.birthDate[MAX_BDATE_LEN-1] = '\0';
    u.passwordHash = hashPassword(password);
    us.count++;
    saveUserStore(us);
    return nullptr;
}

const User* loginUser(const UserStore& us, const char* email, const char* password) {
    unsigned int h = hashPassword(password);
    for (int i = 0; i < us.count; i++) {
        if (strcmp(us.users[i].email, email) == 0 && us.users[i].passwordHash == h)
            return &us.users[i];
    }
    return nullptr;
}

bool saveContactsForUser(int userId, const ContactStore& store) {
    char path[64];
    snprintf(path, sizeof(path), "contacts_%d.dat", userId);
    FILE* f = fopen(path, "wb");
    if (!f) return false;
    fwrite(&store, sizeof(ContactStore), 1, f);
    fclose(f);
    return true;
}

bool loadContactsForUser(int userId, ContactStore& store) {
    char path[64];
    snprintf(path, sizeof(path), "contacts_%d.dat", userId);
    FILE* f = fopen(path, "rb");
    if (!f) { initContactStore(store); return false; }
    bool ok = (fread(&store, sizeof(ContactStore), 1, f) == 1);
    fclose(f);
    if (!ok) initContactStore(store);
    return ok;
}

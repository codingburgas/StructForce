/*
 * Auth Layer — StructForce Contact Management System
 * User accounts, password hashing, binary file persistence
 * Included by presentation.h; never by data.h or logic.h
 */

#pragma once
#include "data.h"
#include <cstring>

const int MAX_USERS     = 100;
const int MAX_FNAME_LEN = 64;
const int MAX_LNAME_LEN = 64;
const int MAX_UMAIL_LEN = 128;
const int MAX_BDATE_LEN = 16;   // DD.MM.YYYY
const int MAX_PASS_LEN  = 128;

struct User {
    int          id;
    char         firstName[MAX_FNAME_LEN];
    char         lastName [MAX_LNAME_LEN];
    char         email    [MAX_UMAIL_LEN];
    char         birthDate[MAX_BDATE_LEN];
    unsigned int passwordHash;
};

struct UserStore {
    User users[MAX_USERS];
    int  count;
    int  nextId;
};

// djb2 hash — never store plaintext passwords
unsigned int hashPassword(const char* password);

// UserStore lifecycle
void        initUserStore(UserStore& us);
bool        saveUserStore(const UserStore& us);   // → users.dat
bool        loadUserStore(UserStore& us);          // ← users.dat

// Returns nullptr on success, or a human-readable error string on failure
const char* registerUser(UserStore& us,
                         const char* firstName, const char* lastName,
                         const char* email,     const char* birthDate,
                         const char* password);

// Returns pointer to matched User, or nullptr if credentials are wrong
const User* loginUser(const UserStore& us, const char* email, const char* password);

// Per-user contact file: contacts_{userId}.dat
bool saveContactsForUser(int userId, const ContactStore& store);
bool loadContactsForUser(int userId, ContactStore& store);

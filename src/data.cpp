/*
 * data.cpp
 * Data Layer — StructForce Contact Management System
 *
 * Implements raw storage operations for contacts.
 * No algorithm logic, no UI code  pure data handling only.
 */

#include "data.h"

#include <cstring>
#include <cstdio>


// Safe string copy: always null-terminates dst; copies at most (maxLen-1) chars
static void safeCopy(char* dst, const char* src, int maxLen) {
    if (!src) { dst[0] = '\0'; return; }
    strncpy(dst, src, maxLen - 1);
    dst[maxLen - 1] = '\0';
}

// Return the array index of the contact with the given id, or -1 if not found
static int findIndex(const ContactStore& store, int id) {
    for (int i = 0; i < store.count; i++) {
        if (store.contacts[i].id == id) return i;
    }
    return -1;
}


void initContactStore(ContactStore& store) {
    store.count  = 0;
    store.nextId = 1;
    for (int i = 0; i < MAX_CONTACTS; i++) {
        store.contacts[i] = {};
    }
}


bool addContact(ContactStore& store,
                const char* name,
                const char* phone,
                const char* email,
                const char* group) {
    if (store.count >= MAX_CONTACTS)  return false;
    if (!name  || name[0]  == '\0')   return false;
    if (!phone || phone[0] == '\0')   return false;

    Contact& c = store.contacts[store.count];
    c.id = store.nextId++;

    safeCopy(c.name,  name,  MAX_NAME_LEN);
    safeCopy(c.phone, phone, MAX_PHONE_LEN);
    safeCopy(c.email, email ? email : "", MAX_EMAIL_LEN);

    if (!group || group[0] == '\0')
        safeCopy(c.group, "General", MAX_GROUP_LEN);
    else
        safeCopy(c.group, group, MAX_GROUP_LEN);

    store.count++;
    return true;
}

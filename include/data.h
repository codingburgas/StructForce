/*
 * data.h
 * Data Layer — StructForce Contact Management System
 * Owns the Contact struct and all raw storage operations.
 * Structures:
 *   Contact — a single contact record (name, phone, group)
 *   ContactStore — fixed-capacity array of contacts with bookkeeping
 */

#pragma once

#include <cstring>

const int MAX_CONTACTS   = 500;
const int MAX_NAME_LEN   = 64;
const int MAX_PHONE_LEN  = 32;
const int MAX_EMAIL_LEN  = 64;
const int MAX_GROUP_LEN  = 32;

/*
 * Contact — one person in the address book
 * Fixed-size char arrays only: no dynamic allocation, no std::string
 */
struct Contact {
    int  id;
    char name [MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
    char group[MAX_GROUP_LEN];
};

/*
 * ContactStore — owns the flat array and tracks active count and next ID
 * Lives as a single global-lifetime instance
 */
struct ContactStore {
    Contact contacts[MAX_CONTACTS];
    int     count;
    int     nextId;
};

// Initialization 
void initContactStore(ContactStore& store);

//CRUD Operations

/*
 * Append a new contact
 * name and phone must not be empty strings
 * group may be empty — defaults to "General"
 * Returns true on success, false if store is full or name/phone are empty
 */
bool addContact(ContactStore& store,
                const char* name,
                const char* phone,
                const char* email,
                const char* group);

/*
 * Remove the contact with the given id
 * Shifts remaining contacts left to keep the array contiguous
 * Returns true on success, false if id is not found
 */
bool removeContact(ContactStore& store, int id);

bool updateContact(ContactStore& store, int id,
                   const char* name, const char* phone,
                   const char* email, const char* group);

/*
 * Return a read only pointer to the flat contacts array
 * The pointer is valid until the next mutation, callers must not cache it
 */
const Contact* getContacts(const ContactStore& store);

/*
 * Return the current number of active contacts
 */
int getContactCount(const ContactStore& store);


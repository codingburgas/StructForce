/*
 * logic.cpp
 * Logic Layer — StructForce Contact Management System
 *
 * Implements all algorithms and wrappers.
 * No ImGui calls, no direct I/O — pure computation only.
 */

#include "logic.h"

#include <cstring>
#include <cctype>

// ─── Internal Helpers ─────────────────────────────────────────────────────────

// Convert string to lowercase into dst (capacity maxLen).
static void toLower(const char* src, char* dst, int maxLen) {
    int i = 0;
    for (; src[i] && i < maxLen - 1; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

// Return true if needle is a substring of haystack (both assumed lowercase).
static bool containsSubstr(const char* haystack, const char* needle) {
    if (needle[0] == '\0') return true;
    return strstr(haystack, needle) != nullptr;
}

// Compare two contacts by the given field; returns strcmp-style result.
static int compareContacts(const Contact& a, const Contact& b, SortField field) {
    if (field == SORT_BY_PHONE) return strcmp(a.phone, b.phone);
    return strcmp(a.name, b.name);
}

// ─── Sorting ──────────────────────────────────────────────────────────────────

void buildIndexArray(int* out, int n) {
    for (int i = 0; i < n; i++) out[i] = i;
}

void quickSort(const ContactStore& store,
               int* indices, int left, int right,
               SortField field, SortOrder order) {
    if (left >= right) return;

    // Use middle element as pivot to reduce worst-case on sorted input
    int mid = left + (right - left) / 2;
    int tmp = indices[mid]; indices[mid] = indices[right]; indices[right] = tmp;

    const Contact& pivot = store.contacts[indices[right]];
    int s = left - 1;

    for (int j = left; j < right; j++) {
        int cmp = compareContacts(store.contacts[indices[j]], pivot, field);
        bool take = (order == SORT_ASC) ? (cmp <= 0) : (cmp >= 0);
        if (take) {
            s++;
            int t = indices[s]; indices[s] = indices[j]; indices[j] = t;
        }
    }

    // Place pivot at its final sorted position
    int t = indices[s + 1]; indices[s + 1] = indices[right]; indices[right] = t;
    int p = s + 1;

    quickSort(store, indices, left,  p - 1, field, order);
    quickSort(store, indices, p + 1, right, field, order);
}

void getSortedIndices(const ContactStore& store,
                      int* outIndices,
                      SortField field, SortOrder order) {
    int n = store.count;
    buildIndexArray(outIndices, n);
    if (n > 1)
        quickSort(store, outIndices, 0, n - 1, field, order);
}

// ─── Searching ────────────────────────────────────────────────────────────────

SearchResult searchContacts(const ContactStore& store, const char* query) {
    SearchResult result;
    result.count = 0;

    char lq[MAX_NAME_LEN + MAX_PHONE_LEN];
    toLower(query ? query : "", lq, sizeof(lq));

    const Contact* contacts = getContacts(store);
    int n = getContactCount(store);

    for (int i = 0; i < n; i++) {
        if (lq[0] == '\0') {
            result.indices[result.count++] = i;
            continue;
        }
        char ln[MAX_NAME_LEN], lp[MAX_PHONE_LEN];
        toLower(contacts[i].name,  ln, MAX_NAME_LEN);
        toLower(contacts[i].phone, lp, MAX_PHONE_LEN);

        if (containsSubstr(ln, lq) || containsSubstr(lp, lq))
            result.indices[result.count++] = i;
    }

    return result;
}

// ─── Recursion ────────────────────────────────────────────────────────────────

int countContactsRecursive(const ContactStore& store, int idx) {
    if (idx >= store.count) return 0;
    return 1 + countContactsRecursive(store, idx + 1);
}

// ─── Analytics Helpers ────────────────────────────────────────────────────────

int getGroupCount(const ContactStore& store) {
    char seen[MAX_CONTACTS][MAX_GROUP_LEN];
    int  seenCount = 0;

    const Contact* contacts = getContacts(store);
    int n = getContactCount(store);

    for (int i = 0; i < n; i++) {
        bool found = false;
        for (int j = 0; j < seenCount; j++) {
            if (strcmp(seen[j], contacts[i].group) == 0) { found = true; break; }
        }
        if (!found && seenCount < MAX_CONTACTS) {
            strncpy(seen[seenCount], contacts[i].group, MAX_GROUP_LEN - 1);
            seen[seenCount][MAX_GROUP_LEN - 1] = '\0';
            seenCount++;
        }
    }
    return seenCount;
}

void getMostCommonGroup(const ContactStore& store, char* outGroup) {
    outGroup[0] = '\0';
    int n = getContactCount(store);
    if (n == 0) return;

    const Contact* contacts = getContacts(store);

    char groups[MAX_CONTACTS][MAX_GROUP_LEN];
    int  counts[MAX_CONTACTS];
    int  unique = 0;

    for (int i = 0; i < n; i++) {
        bool found = false;
        for (int j = 0; j < unique; j++) {
            if (strcmp(groups[j], contacts[i].group) == 0) {
                counts[j]++; found = true; break;
            }
        }
        if (!found && unique < MAX_CONTACTS) {
            strncpy(groups[unique], contacts[i].group, MAX_GROUP_LEN - 1);
            groups[unique][MAX_GROUP_LEN - 1] = '\0';
            counts[unique++] = 1;
        }
    }

    int best = 0;
    for (int i = 1; i < unique; i++)
        if (counts[i] > counts[best]) best = i;

    strncpy(outGroup, groups[best], MAX_GROUP_LEN - 1);
    outGroup[MAX_GROUP_LEN - 1] = '\0';
}

// ─── Validation ───────────────────────────────────────────────────────────────

const char* validateContact(const char* name, const char* phone) {
    if (!name  || name[0]  == '\0') return "Name is required.";
    if (!phone || phone[0] == '\0') return "Phone number is required.";

    bool hasLetter = false;
    for (int i = 0; name[i]; i++)
        if (isalpha((unsigned char)name[i])) { hasLetter = true; break; }
    if (!hasLetter) return "Name must contain at least one letter.";

    bool hasDigit = false;
    for (int i = 0; phone[i]; i++)
        if (isdigit((unsigned char)phone[i])) { hasDigit = true; break; }
    if (!hasDigit) return "Phone number must contain at least one digit.";

    return nullptr;
}

// ─── Presentation-Facing Wrappers ─────────────────────────────────────────────

const char* addContactValidated(ContactStore& store,
                                const char* name,
                                const char* phone,
                                const char* email,
                                const char* group) {
    const char* err = validateContact(name, phone);
    if (err) return err;

    if (!addContact(store, name, phone, email, group))
        return "Contact list is full.";

    return nullptr;
}

const char* updateContactValidated(ContactStore& store, int id,
                                   const char* name,
                                   const char* phone,
                                   const char* email,
                                   const char* group) {
    const char* err = validateContact(name, phone);
    if (err) return err;
    if (!updateContact(store, id, name, phone, email, group))
        return "Contact not found.";
    return nullptr;
}

bool deleteContactById(ContactStore& store, int id) {
    return removeContact(store, id);
}

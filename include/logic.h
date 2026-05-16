/*
 * logic.h
 * Logic Layer — StructForce Contact Management System
 *
 * All algorithm declarations live here.
 * The Logic layer is the ONLY bridge between Presentation and Data.
 * Presentation includes this header; it never includes data.h directly.
 *
 * Algorithms:
 *   - Quick Sort      (contacts by name or phone, ASC / DESC)
 *   - Linear Search   (case-insensitive substring, name or phone)
 *   - Recursive count (countContactsRecursive)
 *
 * Presentation-facing wrappers:
 *   - addContactValidated   (validate → data layer)
 *   - deleteContactById     (data layer delete)
 *   - getGroupCount         (unique group names)
 *   - getMostCommonGroup    (most frequent group label)
 */

#pragma once

#include "data.h"

//Enumerations 

enum SortField {
    SORT_BY_NAME  = 0,
    SORT_BY_PHONE = 1
};

enum SortOrder {
    SORT_ASC  = 0,
    SORT_DESC = 1
};

// Structures 
/*
 * SearchResult — list of indices into ContactStore.contacts[] that matched.
 * Avoids copying contacts; the indices are valid until the next mutation.
 */
struct SearchResult {
    int indices[MAX_CONTACTS];
    int count;
};

// Sorting
/*
 * Fill out[0..n-1] with 0,1,...,n-1.
 * Call this before quickSort to build the initial index array.
 */
void buildIndexArray(int* out, int n);

/*
 * In-place Quick Sort on an index array.
 *   store        — provides contact data for comparisons
 *   indices      — array of indices into store.contacts[] being sorted
 *   left, right  — inclusive bounds of the current partition
 *   field        — SORT_BY_NAME or SORT_BY_PHONE
 *   order        — SORT_ASC or SORT_DESC
 */
void quickSort(const ContactStore& store,
               int* indices, int left, int right,
               SortField field, SortOrder order);

/*
 * Convenience wrapper: build index array then sort it.
 * outIndices must have capacity >= store.count.
 */
void getSortedIndices(const ContactStore& store,
                      int* outIndices,
                      SortField field, SortOrder order);

// ─── Searching ────────────────────────────────────────────────────────────────

/*
 * Case-insensitive substring search across name AND phone fields.
 * An empty query returns all contacts (count == store.count).
 * Returns a SearchResult with matching indices.
 */
SearchResult searchContacts(const ContactStore& store, const char* query);

// ─── Recursion ────────────────────────────────────────────────────────────────

/*
 * Recursively count contacts in store.contacts[idx .. store.count-1].
 * Base case: idx >= store.count → returns 0.
 * Called with idx=0 to count the full store.
 */
int countContactsRecursive(const ContactStore& store, int idx);

// ─── Analytics Helpers ────────────────────────────────────────────────────────

/*
 * Count how many distinct group values exist in store.
 */
int getGroupCount(const ContactStore& store);

/*
 * Write the most frequently occurring group name into outGroup (capacity MAX_GROUP_LEN).
 * If store is empty, writes an empty string.
 */
void getMostCommonGroup(const ContactStore& store, char* outGroup);

// ─── Validation ───────────────────────────────────────────────────────────────

/*
 * Validate contact fields before insertion.
 * Returns nullptr if all fields are valid.
 * Returns a human-readable error string on failure.
 */
const char* validateContact(const char* name, const char* phone);

// ─── Presentation-Facing Wrappers ─────────────────────────────────────────────
// Presentation calls ONLY these — never data layer functions directly.

/*
 * Validate then add a contact via the data layer.
 * Returns nullptr on success, or an error string on failure.
 */
const char* addContactValidated(ContactStore& store,
                                const char* name,
                                const char* phone,
                                const char* email,
                                const char* group);

const char* updateContactValidated(ContactStore& store, int id,
                                   const char* name,
                                   const char* phone,
                                   const char* email,
                                   const char* group);

/*
 * Delete a contact by id via the data layer.
 * Returns true on success, false if id not found.
 */
bool deleteContactById(ContactStore& store, int id);

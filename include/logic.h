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
 

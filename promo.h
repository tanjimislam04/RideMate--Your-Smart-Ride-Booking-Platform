// File: promo.h
// Description: Manages promo codes and discounts for the RideMate system.

#ifndef PROMO_H
#define PROMO_H

// --- The Promo Struct (Node for Linked List) ---
typedef struct PromoNode
{
    char code[20];          // The code the user types, e.g., "SAVE10"
    float discountPercent;  // The discount value, e.g., 10.0 for 10%
    int isActive;           // Admin can turn this on (1) or off (0)
    struct PromoNode *next; // Pointer to the next promo in the list
} Promo;

// --- Function Prototypes ---

// Loads all promo codes from "promos.csv" into the linked list
void loadPromos(Promo **head);

// Saves the entire linked list of promo codes to "promos.csv"
void savePromos(Promo *head);

// Finds an active promo by its code. Returns NULL if not found or inactive.
// This is the main function the rental module will use.
Promo *findActivePromoByCode(Promo *head, const char *code);

// The entry point for the admin's promo management menu
void adminPromoMenu(Promo **head);

// Frees all memory used by the promo list
void freePromoList(Promo **head);

#endif // PROMO_H

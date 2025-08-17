// File: promo.c
// Description: Implements the logic for managing and applying promo codes.

#include "promo.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMO_FILE "data/promos.csv"

// --- Helper to create CSV file with a header if it doesn't exist ---
static void ensurePromoFileExists()
{
    FILE *f = fopen(PROMO_FILE, "r");
    if (f)
    {
        fclose(f);
        return; // File already exists
    }

    f = fopen(PROMO_FILE, "w");
    if (f)
    {
        fprintf(f, "code,discountPercent,isActive\n");
        fclose(f);
    }
}

// --- CSV and Linked List Logic ---

static Promo *parsePromoCSV(char *line)
{
    Promo *p = (Promo *)malloc(sizeof(Promo));
    if (!p)
        return NULL;
    p->next = NULL;
    // Format: code,discountPercent,isActive
    sscanf(line, "%19[^,],%f,%d", p->code, &p->discountPercent, &p->isActive);
    return p;
}

void loadPromos(Promo **head)
{
    ensurePromoFileExists();
    FILE *f = fopen(PROMO_FILE, "r");
    if (!f)
    {
        *head = NULL;
        return;
    }

    char line[128];
    fgets(line, sizeof(line), f); // Skip header

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0)
        {
            Promo *p = parsePromoCSV(line);
            if (p)
            {
                p->next = *head;
                *head = p;
            }
        }
    }
    fclose(f);
}

void savePromos(Promo *head)
{
    FILE *f = fopen(PROMO_FILE, "w");
    if (!f)
    {
        printf("Error: Could not save promo data!\n");
        return;
    }
    fprintf(f, "code,discountPercent,isActive\n");
    for (Promo *p = head; p; p = p->next)
    {
        fprintf(f, "%s,%.2f,%d\n", p->code, p->discountPercent, p->isActive);
    }
    fclose(f);
}

// --- Core Logic ---

Promo *findActivePromoByCode(Promo *head, const char *code)
{
    for (Promo *p = head; p; p = p->next)
    {
        if (p->isActive && strcmp(p->code, code) == 0)
        {
            return p; // Found an active code
        }
    }
    return NULL; // Not found or not active
}

// --- Admin Helper Functions (private to this file) ---

static void addPromo(Promo **head)
{
    Promo *newPromo = (Promo *)malloc(sizeof(Promo));
    if (!newPromo)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("\n--- Create New Promo Code ---\n");
    getStringInput("Enter new promo code (e.g., EID25): ", newPromo->code, 20);
    newPromo->discountPercent = getFloatInput("Enter discount percentage (e.g., 10.5): ", 0.1, 100.0);
    newPromo->isActive = 1; // Active by default

    // Insert at head and save
    newPromo->next = *head;
    *head = newPromo;
    savePromos(*head);
    printf("Promo code '%s' for %.1f%% discount created successfully!\n", newPromo->code, newPromo->discountPercent);
}

static void listAllPromos(Promo *head)
{
    printf("\n--- All Promo Codes ---\n");
    printf("%-20s %-15s %-10s\n", "Code", "Discount (%)", "Status");
    printf("---------------------------------------------\n");
    if (!head)
    {
        printf("No promo codes found.\n");
        return;
    }
    for (Promo *p = head; p; p = p->next)
    {
        printf("%-20s %-15.2f %-10s\n", p->code, p->discountPercent, p->isActive ? "Active" : "Inactive");
    }
}

static void togglePromoActive(Promo *head)
{
    char code[20];
    listAllPromos(head);
    getStringInput("\nEnter promo code to activate/deactivate: ", code, 20);

    // We need to find the code regardless of its active status
    Promo *target = NULL;
    for (Promo *current = head; current; current = current->next)
    {
        if (strcmp(current->code, code) == 0)
        {
            target = current;
            break;
        }
    }

    if (target)
    {
        target->isActive = !target->isActive; // Flip the status (1 becomes 0, 0 becomes 1)
        savePromos(head);
        printf("Promo code '%s' is now %s.\n", target->code, target->isActive ? "ACTIVE" : "INACTIVE");
    }
    else
    {
        printf("Promo code '%s' not found.\n", code);
    }
}

// --- The Public Admin Menu ---

void adminPromoMenu(Promo **head)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Promo Code Management ---\n");
        printf("1. Create New Promo Code\n");
        printf("2. List All Promo Codes\n");
        printf("3. Activate / Deactivate a Code\n");
        printf("4. Back to Admin Panel\n");
        int choice = getIntegerInput("Enter choice: ", 1, 4);

        switch (choice)
        {
        case 1:
            addPromo(head);
            break;
        case 2:
            listAllPromos(*head);
            break;
        case 3:
            togglePromoActive(*head);
            break;
        case 4:
            running = 0;
            break;
        }
        if (running)
            pressEnterToContinue();
    }
}

// --- Memory Management ---

void freePromoList(Promo **head)
{
    Promo *current = *head;
    Promo *nextNode;
    while (current != NULL)
    {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
    *head = NULL;
}

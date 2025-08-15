// File: search.c
// The actual logic for searching and filtering our linked lists.

#include <stdio.h>
#include <string.h>
#define MAX_STRING 100

#include "search.h"
#include "utils.h"

#include "vehicle.h"
#include "customer.h"
#include "rental.h"

// --- Vehicle Functions ---

void searchVehiclesByText(Vehicle *head, const char *query)
{
    printf("\n--- Search Results for '%s' ---\n", query);
    int found = 0;
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->active)
        {
            // A real pro version would make this case-insensitive!
            if (strstr(v->make, query) || strstr(v->model, query))
            {
                displayVehicle(v); // This function is in vehicle.c
                found = 1;
            }
        }
    }
    if (!found)
    {
        printf("No vehicles found matching your query.\n");
    }
}

void filterVehiclesByType(Vehicle *head, VehicleType type)
{
    printf("\n--- Vehicles of Type: %s ---\n", vehicleTypeStr(type)); // vehicleTypeStr from vehicle.c
    int found = 0;
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->active && v->type == type)
        {
            displayVehicle(v);
            found = 1;
        }
    }
    if (!found)
    {
        printf("No vehicles found of this type.\n");
    }
}

void filterVehiclesByPrice(Vehicle *head, float maxPrice)
{
    printf("\n--- Vehicles with Daily Rate under $%.2f ---\n", maxPrice);
    int found = 0;
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->active && v->ratePerDay <= maxPrice)
        {
            displayVehicle(v);
            found = 1;
        }
    }
    if (!found)
    {
        printf("No vehicles found in this price range.\n");
    }
}

// --- Rental Functions ---

void searchRentalsByCustomerId(Rental *head, int customerId)
{
    printf("\n--- Searching rentals for customer ID %d ---\n", customerId);
    int found = 0;
    for (Rental *r = head; r; r = r->next)
    {
        if (r->customerId == customerId)
        {
            displayRental(r); // Assumes displayRental exists in rental.c
            found = 1;
        }
    }
    if (!found)
    {
        printf("No rentals found for this customer.\n");
    }
}

// --- The Admin Menu for this Module ---

void adminSearchMenu(Vehicle *vehicleHead, Rental *rentalHead)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Advanced Search & Filter ---\n");
        printf("1. Search Vehicle by Name/Model\n");
        printf("2. Filter Vehicles by Type\n");
        printf("3. Filter Vehicles by Max Price\n");
        printf("4. Find Rentals by Customer ID\n");
        printf("5. Back to Admin Panel\n");
        int choice = getIntegerInput("Enter choice: ", 1, 5);

        switch (choice)
        {
        case 1:
        {
            char query[MAX_STRING]; // This line will now work!
            getStringInput("Enter make or model to search: ", query, MAX_STRING);
            searchVehiclesByText(vehicleHead, query);
            break;
        }
        case 2:
        {
            printf("Select Type: 0=CAR, 1=MOTORCYCLE, 2=TRUCK, 3=VAN\n");
            int type = getIntegerInput("Enter type (0-3): ", 0, 3);
            filterVehiclesByType(vehicleHead, (VehicleType)type);
            break;
        }
        case 3:
        {
            float price = getFloatInput("Enter maximum daily rate: ", 1.0, 5000.0);
            filterVehiclesByPrice(vehicleHead, price);
            break;
        }
        case 4:
        {
            int custId = getIntegerInput("Enter Customer ID to find rentals for: ", 1000, 9999);
            searchRentalsByCustomerId(rentalHead, custId);
            break;
        }
        case 5:
            running = 0;
            break;
        }
        if (running)
        {
            pressEnterToContinue();
        }
    }
}
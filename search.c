#include <stdio.h>
#include <string.h>

#include "search.h"
#include "utils.h"
#include "vehicle.h"
#include "customer.h"
#include "rental.h"

void searchVehiclesByText(const Vehicle *head, const char *query)
{
    printf("\n--- Search Results for '%s' ---\n", query);
    int found = 0;
    for (const Vehicle *v = head; v; v = v->next)
    {
        if (v->active)
        {
            if (strstr(v->make, query) || strstr(v->model, query))
            {
                displayVehicle(v);
                found = 1;
            }
        }
    }
    if (!found)
    {
        printf("No vehicles found matching your query.\n");
    }
}

void filterVehiclesByType(const Vehicle *head, const char *type)
{
    printf("\n--- Vehicles of Type: %s ---\n", type);
    int found = 0;
    for (const Vehicle *v = head; v; v = v->next)
    {
        if (v->active && strcmp(vehicleTypeStr(v->type), type) == 0)
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

void filterVehiclesByPrice(const Vehicle *head, float maxPrice)
{
    printf("\n--- Vehicles with Daily Rate under $%.2f ---\n", maxPrice);
    int found = 0;
    for (const Vehicle *v = head; v; v = v->next)
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

void searchRentalsByCustomerId(const Rental *head, int customerId)
{
    printf("\n--- Searching rentals for customer ID %d ---\n", customerId);
    int found = 0;
    for (const Rental *r = head; r; r = r->next)
    {
        if (r->customerId == customerId)
        {
            displayRental(r);
            found = 1;
        }
    }
    if (!found)
    {
        printf("No rentals found for this customer.\n");
    }
}

int compareVehicles(const Vehicle *a, const Vehicle *b, VehicleSortField field, SortOrder order)
{
    float diff = 0;

    switch (field)
    {
    case SORT_PRICE_DAY:
        diff = a->ratePerDay - b->ratePerDay;
        break;
    case SORT_PRICE_HOUR:
        diff = a->ratePerHour - b->ratePerHour;
        break;
    case SORT_YEAR:
        diff = (float)(a->year - b->year);
        break;
    }

    if (order == SORT_ASC)
    {
        return diff > 0;
    }
    else
    {
        return diff < 0;
    }
}

void sortVehicles(Vehicle **head, VehicleSortField field, SortOrder order)
{
    if (head == NULL || *head == NULL || (*head)->next == NULL)
    {
        return;
    }

    int swapped;
    Vehicle *current;

    do
    {
        swapped = 0;
        current = *head;

        while (current->next != NULL)
        {
            if (compareVehicles(current, current->next, field, order))
            {

                Vehicle temp;
                Vehicle *nextNode = current->next;
                temp.id = current->id;
                strcpy(temp.make, current->make);
                strcpy(temp.model, current->model);
                temp.year = current->year;
                temp.type = current->type;
                temp.ratePerDay = current->ratePerDay;
                temp.ratePerHour = current->ratePerHour;
                temp.active = current->active;
                temp.available = current->available;

                current->id = nextNode->id;
                strcpy(current->make, nextNode->make);
                strcpy(current->model, nextNode->model);
                current->year = nextNode->year;
                current->type = nextNode->type;
                current->ratePerDay = nextNode->ratePerDay;
                current->ratePerHour = nextNode->ratePerHour;
                current->active = nextNode->active;
                current->available = nextNode->available;

                nextNode->id = temp.id;
                strcpy(nextNode->make, temp.make);
                strcpy(nextNode->model, temp.model);
                nextNode->year = temp.year;
                nextNode->type = temp.type;
                nextNode->ratePerDay = temp.ratePerDay;
                nextNode->ratePerHour = temp.ratePerHour;
                nextNode->active = temp.active;
                nextNode->available = temp.available;

                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);

    printf("\nList has been sorted.\n");
}

void adminSearchMenu(Vehicle *vehicleHead, Rental *rentalHead)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Advanced Search, Filter & Sort ---\n");
        printf("1. Search Vehicle by Name/Model\n");
        printf("2. Filter Vehicles by Type\n");
        printf("3. Filter Vehicles by Max Price\n");
        printf("4. Sort Vehicles\n");
        printf("5. Find Rentals by Customer ID\n");
        printf("6. Back to Admin Panel\n");
        int choice = getIntegerInput("Enter your choice: ", 1, 6);

        switch (choice)
        {
        case 1:
        {
            char query[MAX_STRING];
            getStringInput("Enter make or model to search: ", query, MAX_STRING);
            searchVehiclesByText(vehicleHead, query);
            break;
        }
        case 2:
        {
            printf("Select Type: 0=CAR, 1=MOTORCYCLE, 2=TRUCK, 3=VAN\n");
            int type = getIntegerInput("Enter type (0-3): ", 0, 3);
            filterVehiclesByType(vehicleHead, vehicleTypeStr((VehicleType)type));
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
            printf("\n--- Sort Vehicles ---\n");
            int sortBy = getIntegerInput("Sort by (1=Price/Day, 2=Year): ", 1, 2);
            int order = getIntegerInput("Order (1=Ascending, 0=Descending): ", 0, 1);

            VehicleSortField field = (sortBy == 1) ? SORT_PRICE_DAY : SORT_YEAR;
            SortOrder sortOrder = (order == 1) ? SORT_ASC : SORT_DESC;

            sortVehicles(&vehicleHead, field, sortOrder);

            listAllVehicles(vehicleHead);
            break;
        }
        case 5:
        {
            int custId = getIntegerInput("Enter Customer ID to find rentals for: ", 1000, 9999);
            searchRentalsByCustomerId(rentalHead, custId);
            break;
        }
        case 6:
            running = 0;
            break;
        }
        if (running)
        {
            pressEnterToContinue();
        }
    }
}
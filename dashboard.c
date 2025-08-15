// File: dashboard.c
// Description: Implements the logic for calculating and displaying business statistics.

#include "dashboard.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "utils.h"
#include "vehicle.h"
#include "customer.h"
#include "rental.h"

// --- Helper function to draw a simple ASCII bar ---
static void printBar(float value, float maxValue)
{
    int barWidth = 40; // Max width of the bar in characters
    if (maxValue == 0)
    {
        printf("[No data]");
        return;
    }
    int width = (int)((value / maxValue) * barWidth);
    for (int i = 0; i < width; i++)
    {
        printf("#");
    }
}

void showAdminDashboard(Vehicle *vehicleHead, Customer *customerHead, Rental *rentalHead)
{
    // --- 1. Initialize all counters ---
    int totalVehicles = 0, availableVehicles = 0, rentedVehicles = 0;
    int totalCustomers = 0;
    int totalRentals = 0, activeRentals = 0;
    float totalRevenue = 0.0f;
    float monthlyRevenue[12] = {0}; // An array to hold revenue for each month (Jan=0, Feb=1, ...)

    // --- 2. Calculate Vehicle Statistics ---
    for (Vehicle *v = vehicleHead; v; v = v->next)
    {
        if (v->active)
        {
            totalVehicles++;
            if (v->available)
            {
                availableVehicles++;
            }
            else
            {
                rentedVehicles++;
            }
        }
    }

    // --- 3. Calculate Customer Statistics ---
    for (Customer *c = customerHead; c; c = c->next)
    {
        if (c->active)
        {
            totalCustomers++;
        }
    }

    // --- 4. Calculate Rental and Revenue Statistics ---
    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    int currentYear = localTime->tm_year + 1900;

    for (Rental *r = rentalHead; r; r = r->next)
    {
        totalRentals++;
        if (r->status == RENT_ACTIVE)
        {
            activeRentals++;
        }

        if (r->status == RENT_COMPLETED)
        {
            totalRevenue += r->totalCost;

            // For the bar chart, let's only consider rentals from the current year
            int year, month, day;
            if (sscanf(r->startTime, "%d-%d-%d", &year, &month, &day) == 3)
            {
                if (year == currentYear)
                {
                    int monthIndex = month - 1;
                    if (monthIndex >= 0 && monthIndex < 12)
                    {
                        monthlyRevenue[monthIndex] += r->totalCost;
                    }
                }
            }
        }
    }

    // --- 5. Display the Dashboard ---
    clearScreen();
    printf("=============================================================\n");
    printf("                  ADMIN DASHBOARD (As of %d)\n", currentYear);
    printf("=============================================================\n\n");

    printf("--- FLEET STATUS ------------------------------------------\n");
    printf("Total Vehicles: %d\n", totalVehicles);
    printf("  - Available for Rent: %d\n", availableVehicles);
    printf("  - Currently Rented:   %d\n", rentedVehicles);
    printf("\n");

    printf("--- CUSTOMER & RENTAL STATS -------------------------------\n");
    printf("Total Active Customers: %d\n", totalCustomers);
    printf("Total Rentals (All Time): %d\n", totalRentals);
    printf("Currently Active Rentals: %d\n", activeRentals);
    printf("\n");

    printf("--- FINANCIAL OVERVIEW ------------------------------------\n");
    printf("Total Revenue (from completed rentals): $%.2f\n\n", totalRevenue);

    printf("--- Monthly Revenue for %d (Bar Chart) ---\n", currentYear);
    float maxMonthRevenue = 0.0f;
    for (int i = 0; i < 12; i++)
    {
        if (monthlyRevenue[i] > maxMonthRevenue)
        {
            maxMonthRevenue = monthlyRevenue[i];
        }
    }

    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; i++)
    {
        printf("%-3s | ", months[i]);
        printBar(monthlyRevenue[i], maxMonthRevenue);
        printf(" $%.2f\n", monthlyRevenue[i]);
    }
    printf("-------------------------------------------------------------\n");
}
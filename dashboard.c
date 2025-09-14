#include "dashboard.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "utils.h"
#include "vehicle.h"
#include "customer.h"
#include "rental.h"

static void printBar(float value, float maxValue)
{
    int barWidth = 40;
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
    int totalVehicles = 0, availableVehicles = 0, rentedVehicles = 0;
    int totalCustomers = 0;
    int totalRentals = 0, activeRentals = 0;
    float totalRevenue = 0.0f;
    float monthlyRevenue[12] = {0};

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

    for (Customer *c = customerHead; c; c = c->next)
    {
        if (c->active)
        {
            totalCustomers++;
        }
    }

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

void showBookingCalendar(Vehicle *vehicleHead, Rental *rentalHead)
{
    clearScreen();
    printf("\n--- Vehicle Booking Calendar ---\n");
    listAllVehicles(vehicleHead);

    int vehicleId = getIntegerInput("\nEnter Vehicle ID to see its calendar: ", 1, 9999);
    Vehicle *v = findVehicleById(vehicleHead, vehicleId);
    if (!v)
    {
        printf("Vehicle not found.\n");
        return;
    }

    int year = getIntegerInput("Enter Year (e.g., 2025): ", 2020, 2030);
    int month = getIntegerInput("Enter Month (1-12): ", 1, 12);

    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        daysInMonth[2] = 29;
    }

    struct tm firstDayOfMonth = {0};
    firstDayOfMonth.tm_year = year - 1900;
    firstDayOfMonth.tm_mon = month - 1;
    firstDayOfMonth.tm_mday = 1;
    mktime(&firstDayOfMonth);
    int startingDay = firstDayOfMonth.tm_wday;

    int booked_days[32] = {0};
    for (int day = 1; day <= daysInMonth[month]; day++)
    {
        struct tm currentDayTm = {0};
        currentDayTm.tm_year = year - 1900;
        currentDayTm.tm_mon = month - 1;
        currentDayTm.tm_mday = day;
        time_t currentDayTime = mktime(&currentDayTm);

        for (Rental *r = rentalHead; r; r = r->next)
        {
            if (r->vehicleId == vehicleId && r->status == RENT_ACTIVE)
            {
                time_t rentalStart, rentalEnd;
                if (stringToTime(r->startTime, &rentalStart) && stringToTime(r->endTime, &rentalEnd))
                {
                    if (currentDayTime >= rentalStart && currentDayTime <= rentalEnd)
                    {
                        booked_days[day] = 1;
                        break;
                    }
                }
            }
        }
    }

    const char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    printf("\n--- Calendar for %s %s (ID: %d) | %s %d ---\n", v->make, v->model, v->id, months[month - 1], year);
    printf(" 'XX' marks a day the vehicle is booked.\n\n");

    printf("  Su  Mo  Tu  We  Th  Fr  Sa\n");
    printf("-----------------------------\n");

    for (int i = 0; i < startingDay; i++)
    {
        printf("    ");
    }

    for (int day = 1; day <= daysInMonth[month]; day++)
    {
        if (booked_days[day])
        {
            printf(" XX ");
        }
        else
        {
            printf(" %2d ", day);
        }

        if ((day + startingDay) % 7 == 0)
        {
            printf("\n");
        }
    }
    printf("\n-----------------------------\n");
}
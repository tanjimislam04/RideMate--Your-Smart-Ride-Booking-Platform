#include "reports.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "vehicle.h"
#include "customer.h"
#include "rental.h"

typedef struct
{
    int vehicleId;
    int rentalCount;
    char make[MAX_STRING];
    char model[MAX_STRING];
} VehicleUsage;

static void generate_report_filename(char *buffer, int size, const char *prefix)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(buffer, size, "reports/%s_%04d-%02d-%02d.csv", prefix, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
}

static void generateMonthlyRevenueReport(Rental *rentalHead)
{
    printf("\n--- Monthly Revenue Report ---\n");
    int year = getIntegerInput("Enter year to generate report for (e.g., 2025): ", 2020, 2100);

    float monthly_revenue[12] = {0};

    for (Rental *r = rentalHead; r; r = r->next)
    {
        if (r->status == RENT_COMPLETED)
        {
            int y, m;
            if (sscanf(r->startTime, "%d-%d", &y, &m) == 2)
            {
                if (y == year && m >= 1 && m <= 12)
                {
                    monthly_revenue[m - 1] += r->totalCost;
                }
            }
        }
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "reports/revenue_report_%d.csv", year);

    FILE *f = fopen(filename, "w");
    if (!f)
    {
        printf("Error: Could not create report file '%s'\n", filename);
        return;
    }

    printf("\n--- Revenue Report for %d ---\n", year);
    printf("%-10s | %s\n", "Month", "Revenue");
    printf("-------------------------\n");

    fprintf(f, "Month,Revenue\n");
    const char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    for (int i = 0; i < 12; i++)
    {
        fprintf(f, "%s,%.2f\n", months[i], monthly_revenue[i]);
        printf("%-10s | $%.2f\n", months[i], monthly_revenue[i]);
    }

    fclose(f);
    printf("\nSuccessfully generated monthly revenue report: '%s'\n", filename);
}

static int compareVehicleUsage(const void *a, const void *b)
{
    const VehicleUsage *usageA = (const VehicleUsage *)a;
    const VehicleUsage *usageB = (const VehicleUsage *)b;
    return usageB->rentalCount - usageA->rentalCount;
}

static void generateTopVehiclesReport(Rental *rentalHead, Vehicle *vehicleHead)
{
    printf("\n--- Top Rented Vehicles Report ---\n");

    int vehicle_count = 0;
    for (Vehicle *v = vehicleHead; v; v = v->next)
    {
        if (v->active)
            vehicle_count++;
    }

    if (vehicle_count == 0)
    {
        printf("No vehicles in the system to report on.\n");
        return;
    }

    VehicleUsage *usage_stats = (VehicleUsage *)malloc(vehicle_count * sizeof(VehicleUsage));
    int count = 0;
    for (Vehicle *v = vehicleHead; v; v = v->next)
    {
        if (v->active)
        {
            usage_stats[count].vehicleId = v->id;
            strncpy(usage_stats[count].make, v->make, MAX_STRING - 1);
            usage_stats[count].make[MAX_STRING - 1] = '\0';
            strncpy(usage_stats[count].model, v->model, MAX_STRING - 1);
            usage_stats[count].model[MAX_STRING - 1] = '\0';
            usage_stats[count].rentalCount = 0;
            count++;
        }
    }

    for (Rental *r = rentalHead; r; r = r->next)
    {
        for (int i = 0; i < vehicle_count; i++)
        {
            if (r->vehicleId == usage_stats[i].vehicleId)
            {
                usage_stats[i].rentalCount++;
                break;
            }
        }
    }

    qsort(usage_stats, vehicle_count, sizeof(VehicleUsage), compareVehicleUsage);

    char filename[128];
    generate_report_filename(filename, sizeof(filename), "top_vehicles_report");

    FILE *f = fopen(filename, "w");
    if (!f)
    {
        printf("Error: Could not create report file '%s'\n", filename);
        free(usage_stats);
        return;
    }

    printf("\n--- Top Rented Vehicles (All Time) ---\n");
    printf("%-5s %-10s %-15s %-15s\n", "Rank", "Vehicle ID", "Model", "Times Rented");
    printf("---------------------------------------------------\n");

    fprintf(f, "Rank,VehicleID,Make,Model,RentalCount\n");
    int top_n = vehicle_count < 10 ? vehicle_count : 10;
    for (int i = 0; i < top_n; i++)
    {
        fprintf(f, "%d,%d,%s,%s,%d\n", i + 1, usage_stats[i].vehicleId, usage_stats[i].make, usage_stats[i].model, usage_stats[i].rentalCount);
        printf("#%-4d %-10d %-15s %-15d\n", i + 1, usage_stats[i].vehicleId, usage_stats[i].model, usage_stats[i].rentalCount);
    }

    fclose(f);
    free(usage_stats);
    printf("\nSuccessfully generated top vehicles report: '%s'\n", filename);
}

void adminReportsMenu(Vehicle *vehicleHead, Customer *customerHead __attribute__((unused)), Rental *rentalHead)
{
    int running = 1;
    system("mkdir reports 2>nul");

    while (running)
    {
        clearScreen();
        printf("\n--- Reports & Analytics ---\n");
        printf("Reports are saved as CSV files in the 'reports' folder.\n\n");
        printf("1. Generate Monthly Revenue Report\n");
        printf("2. Generate Top Rented Vehicles Report\n");
        printf("3. Back to Admin Panel\n");
        int choice = getIntegerInput("Enter choice: ", 1, 3);

        switch (choice)
        {
        case 1:
            generateMonthlyRevenueReport(rentalHead);
            break;
        case 2:
            generateTopVehiclesReport(rentalHead, vehicleHead);
            break;
        case 3:
            running = 0;
            break;
        }
        if (running)
        {
            pressEnterToContinue();
        }
    }
}
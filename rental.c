#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "vehicle.h" // for Vehicle, Route, findVehicleById, route functions, saveVehicles
#include "customer.h"
#include "rental.h"

#define RENTAL_FILE "rentals.csv"

/* ---------- internal helpers ---------- */

static int nextRentalId = 10001;

static void nowString(char *buf, size_t n)
{
    time_t t = time(NULL);
    struct tm *tmv = localtime(&t);
    strftime(buf, n, "%Y-%m-%d %H:%M", tmv);
}

static void futureStringMinutes(int minutes, char *buf, size_t n)
{
    time_t t = time(NULL);
    t += (time_t)minutes * 60;
    struct tm *tmv = localtime(&t);
    strftime(buf, n, "%Y-%m-%d %H:%M", tmv);
}

static void adjustNextId(Rental *head)
{
    int maxId = nextRentalId - 1;
    for (Rental *r = head; r; r = r->next)
        if (r->id > maxId)
            maxId = r->id;
    nextRentalId = maxId + 1;
}

/* ---------- CSV I/O ---------- */

static Rental *parseRentalCSV(char *line)
{
    // CSV: id,customerId,vehicleId,type,startTime,endTime,totalCost,status,routeId
    Rental *r = (Rental *)malloc(sizeof(Rental));
    if (!r)
        return NULL;
    r->next = NULL;

    int id, custId, vehId, type, status, routeId;
    char start[32] = {0}, end[32] = {0};
    float total;

    int n = sscanf(line, "%d,%d,%d,%d,%19[^,],%19[^,],%f,%d,%d",
                   &id, &custId, &vehId, &type, start, end, &total, &status, &routeId);
    if (n != 9)
    {
        free(r);
        return NULL;
    }

    r->id = id;
    r->customerId = custId;
    r->vehicleId = vehId;
    r->type = (RentalType)type;
    strncpy(r->startTime, start, sizeof(r->startTime) - 1);
    r->startTime[sizeof(r->startTime) - 1] = '\0';
    strncpy(r->endTime, end, sizeof(r->endTime) - 1);
    r->endTime[sizeof(r->endTime) - 1] = '\0';
    r->totalCost = total;
    r->status = (RentalStatus)status;
    r->routeId = routeId;

    return r;
}

void loadRentals(Rental **head)
{
    *head = NULL;
    FILE *f = fopen(RENTAL_FILE, "r");
    if (!f)
        return;

    char line[512];
    // skip header if present
    if (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "id,", 3) != 0)
        {
            // first line was a record, rewind
            fseek(f, 0, SEEK_SET);
        }
    }

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        if (!line[0])
            continue;
        Rental *r = parseRentalCSV(line);
        if (r)
        {
            r->next = *head;
            *head = r;
        }
    }
    fclose(f);
    adjustNextId(*head);
}

void saveRentals(Rental *head)
{
    FILE *f = fopen(RENTAL_FILE, "w");
    if (!f)
    {
        printf("Error: could not open %s\n", RENTAL_FILE);
        return;
    }
    fprintf(f, "id,customerId,vehicleId,type,startTime,endTime,totalCost,status,routeId\n");
    for (Rental *r = head; r; r = r->next)
    {
        fprintf(f, "%d,%d,%d,%d,%s,%s,%.2f,%d,%d\n",
                r->id, r->customerId, r->vehicleId, (int)r->type,
                r->startTime, r->endTime, r->totalCost, (int)r->status, r->routeId);
    }
    fclose(f);
}

/* ---------- Find ---------- */

Rental *findRentalById(Rental *head, int rentalId)
{
    for (Rental *cur = head; cur; cur = cur->next)
        if (cur->id == rentalId)
            return cur;
    return NULL;
}

/* ---------- Lifecycle: complete / cancel ---------- */

int completeRental(Rental *r, Vehicle *vehicleHead)
{
    if (!r)
    {
        printf("Error: NULL rental.\n");
        return 0;
    }
    if (r->status != RENT_ACTIVE)
    {
        printf("Rental #%d is not ACTIVE (current status: %d).\n", r->id, (int)r->status);
        return 0;
    }

    // Allow user to override end time
    char actualEnd[32];
    printf("Enter actual end time (YYYY-MM-DD HH:MM) or leave blank to keep [%s]: ",
           r->endTime[0] ? r->endTime : "(empty)");
    getInput("", actualEnd, sizeof(actualEnd));
    if (strlen(actualEnd) == 16 && actualEnd[4] == '-' && actualEnd[7] == '-' &&
        actualEnd[10] == ' ' && actualEnd[13] == ':')
    {
        strncpy(r->endTime, actualEnd, sizeof(r->endTime) - 1);
        r->endTime[sizeof(r->endTime) - 1] = '\0';
    }
    else if (!r->endTime[0])
    {
        nowString(r->endTime, sizeof(r->endTime));
    }

    r->status = RENT_COMPLETED;

    // Free vehicle
    Vehicle *v = findVehicleById(vehicleHead, r->vehicleId);
    if (v)
    {
        v->available = 1;
        saveVehicles(vehicleHead);
    }
    else
    {
        printf("Warning: vehicle #%d not found; continue anyway.\n", r->vehicleId);
    }

    printf("Rental #%d marked COMPLETED. Vehicle #%d is now AVAILABLE.\n",
           r->id, r->vehicleId);
    return 1;
}

int cancelRental(Rental *r, Vehicle *vehicleHead)
{
    if (!r)
    {
        printf("Error: NULL rental.\n");
        return 0;
    }
    if (r->status != RENT_ACTIVE)
    {
        printf("Rental #%d is not ACTIVE (current status: %d).\n", r->id, (int)r->status);
        return 0;
    }

    // set end time to now (or leave as-is)
    nowString(r->endTime, sizeof(r->endTime));
    r->status = RENT_CANCELLED;

    Vehicle *v = findVehicleById(vehicleHead, r->vehicleId);
    if (v)
    {
        v->available = 1;
        saveVehicles(vehicleHead);
    }
    else
    {
        printf("Warning: vehicle #%d not found; continue anyway.\n", r->vehicleId);
    }

    printf("Rental #%d CANCELLED. Vehicle #%d is now AVAILABLE.\n",
           r->id, r->vehicleId);
    return 1;
}

/* ---------- Prompt helpers ---------- */

void completeRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead)
{
    if (!rentalHead)
    {
        printf("No rentals yet.\n");
        return;
    }
    char buf[32];
    getInput("Enter Rental ID to complete: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid ID.\n");
        return;
    }
    int rid = atoi(buf);

    Rental *r = findRentalById(rentalHead, rid);
    if (!r)
    {
        printf("Rental not found.\n");
        return;
    }

    if (completeRental(r, vehicleHead))
    {
        saveRentals(rentalHead);
    }
}

void cancelRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead)
{
    if (!rentalHead)
    {
        printf("No rentals yet.\n");
        return;
    }
    char buf[32];
    getInput("Enter Rental ID to cancel: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid ID.\n");
        return;
    }
    int rid = atoi(buf);

    Rental *r = findRentalById(rentalHead, rid);
    if (!r)
    {
        printf("Rental not found.\n");
        return;
    }

    if (cancelRental(r, vehicleHead))
    {
        saveRentals(rentalHead);
    }
}

/* ---------- Display ---------- */

static const char *typeStr(RentalType t)
{
    switch (t)
    {
    case RENT_HOURLY:
        return "HOURLY";
    case RENT_DAILY:
        return "DAILY";
    case RENT_ROUTE:
        return "ROUTE";
    default:
        return "UNKNOWN";
    }
}
static const char *statusStr(RentalStatus s)
{
    switch (s)
    {
    case RENT_ACTIVE:
        return "ACTIVE";
    case RENT_COMPLETED:
        return "COMPLETED";
    case RENT_CANCELLED:
        return "CANCELLED";
    default:
        return "UNKNOWN";
    }
}

void displayAllRentals(Rental *head)
{
    if (!head)
    {
        printf("No rentals found.\n");
        return;
    }
    printf("\n--- All Rentals ---\n");
    printf("%-6s %-6s %-7s %-7s %-17s %-17s %-10s %-8s\n",
           "ID", "Cust", "Veh", "Type", "Start", "End", "Status", "Cost");
    for (Rental *r = head; r; r = r->next)
    {
        printf("%-6d %-6d %-7d %-7s %-17s %-17s %-10s $%-7.2f\n",
               r->id, r->customerId, r->vehicleId, typeStr(r->type),
               r->startTime, r->endTime, statusStr(r->status), r->totalCost);
        if (r->type == RENT_ROUTE && r->routeId > 0)
        {
            printf("   Route ID: %d\n", r->routeId);
        }
    }
}

void displayRentalsByCustomer(Rental *head, int customerId)
{
    int found = 0;
    printf("\n--- My Rentals ---\n");
    printf("%-6s %-7s %-7s %-17s %-17s %-10s %-7s\n",
           "ID", "Veh", "Type", "Start", "End", "Status", "Cost");
    for (Rental *r = head; r; r = r->next)
    {
        if (r->customerId == customerId)
        {
            printf("%-6d %-7d %-7s %-17s %-17s %-10s $%-7.2f\n",
                   r->id, r->vehicleId, typeStr(r->type),
                   r->startTime, r->endTime, statusStr(r->status), r->totalCost);
            if (r->type == RENT_ROUTE && r->routeId > 0)
            {
                printf("   Route ID: %d\n", r->routeId);
            }
            found = 1;
        }
    }
    if (!found)
        printf("No rentals.\n");
}

/* ---------- Create rental (customer flow) ---------- */

void createRentalByCustomer(Rental **rentalHead, Vehicle *vehicleHead, Customer *current)
{
    if (!current)
    {
        printf("Error: no current customer.\n");
        return;
    }

    // 1) choose vehicle
    displayAvailableVehicles(vehicleHead);
    char buf[32];
    getInput("Enter Vehicle ID to book: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid vehicle ID.\n");
        return;
    }
    int vid = atoi(buf);

    Vehicle *v = findVehicleById(vehicleHead, vid);
    if (!v)
    {
        printf("Vehicle not found.\n");
        return;
    }
    if (!v->active)
    {
        printf("Vehicle is inactive.\n");
        return;
    }
    if (!v->available)
    {
        printf("Sorry, vehicle is not available.\n");
        return;
    }

    // 2) choose rental type
    printf("\nSelect Rental Type:\n");
    printf("1) Hourly (rate: %.2f/hr)\n", v->ratePerHour);
    printf("2) Daily  (rate: %.2f/day)\n", v->ratePerDay);
    printf("3) Route Trip (if routes exist)\n");
    getInput("Choose (1-3): ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid.\n");
        return;
    }
    int tchoice = atoi(buf);
    if (tchoice < 1 || tchoice > 3)
    {
        printf("Invalid.\n");
        return;
    }

    // 3) create rental
    Rental *r = (Rental *)malloc(sizeof(Rental));
    if (!r)
    {
        printf("Memory allocation failed.\n");
        return;
    }
    memset(r, 0, sizeof(*r));
    r->id = nextRentalId++;
    r->customerId = current->id;
    r->vehicleId = v->id;
    r->status = RENT_ACTIVE;
    r->routeId = 0;

    nowString(r->startTime, sizeof(r->startTime));

    if (tchoice == 1)
    {
        // hourly
        r->type = RENT_HOURLY;
        getInput("Enter hours (1-24): ", buf, sizeof(buf));
        if (!isValidNumber(buf))
        {
            printf("Invalid hours.\n");
            free(r);
            return;
        }
        int hours = atoi(buf);
        if (hours < 1 || hours > 24)
        {
            printf("Invalid hours range.\n");
            free(r);
            return;
        }

        r->totalCost = hours * v->ratePerHour;
        futureStringMinutes(hours * 60, r->endTime, sizeof(r->endTime));
    }
    else if (tchoice == 2)
    {
        // daily
        r->type = RENT_DAILY;
        getInput("Enter days (1-30): ", buf, sizeof(buf));
        if (!isValidNumber(buf))
        {
            printf("Invalid days.\n");
            free(r);
            return;
        }
        int days = atoi(buf);
        if (days < 1 || days > 30)
        {
            printf("Invalid days range.\n");
            free(r);
            return;
        }

        r->totalCost = days * v->ratePerDay;
        futureStringMinutes(days * 24 * 60, r->endTime, sizeof(r->endTime));
    }
    else
    {
        // route trip
        r->type = RENT_ROUTE;

        if (!routeHead)
        {
            printf("No routes defined by admin. Cannot book a route trip.\n");
            free(r);
            return;
        }
        printf("\n--- Available Routes ---\n");
        displayAllRoutes(routeHead);

        getInput("Enter Route ID: ", buf, sizeof(buf));
        if (!isValidNumber(buf))
        {
            printf("Invalid Route ID.\n");
            free(r);
            return;
        }
        int rid = atoi(buf);
        Route *route = findRouteById(routeHead, rid);
        if (!route)
        {
            printf("Route not found.\n");
            free(r);
            return;
        }

        r->routeId = route->id;
        r->totalCost = route->baseFare;
        futureStringMinutes(route->etaMin, r->endTime, sizeof(r->endTime));
    }

    // 4) add to list, mark vehicle unavailable, save
    r->next = *rentalHead;
    *rentalHead = r;

    v->available = 0;
    saveVehicles(vehicleHead);
    saveRentals(*rentalHead);

    printf("\nRental created!\n");
    printf("Rental ID: %d | Vehicle: %d | Type: %s | Start: %s | End: %s | Cost: $%.2f | Status: %s\n",
           r->id, r->vehicleId, typeStr(r->type), r->startTime, r->endTime, r->totalCost, statusStr(r->status));
}

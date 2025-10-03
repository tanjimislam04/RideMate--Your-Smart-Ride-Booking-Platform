#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "vehicle.h"
#include "customer.h"
#include "rental.h"
#include "driver.h"
#include "invoice.h"
#include <time.h>
#include <math.h>

#define RENTAL_FILE "rentals.csv"

extern Route *routeHead;

static int isVehicleBooked(Rental *head, int vehicleId, time_t newStart, time_t newEnd)
{
    for (Rental *r = head; r; r = r->next)
    {
        if (r->vehicleId == vehicleId && r->status == RENT_ACTIVE)
        {
            time_t existingStart, existingEnd;

            if (stringToTime(r->startTime, &existingStart) && stringToTime(r->endTime, &existingEnd))
            {
                // Check for overlap: new booking overlaps with existing booking
                if (newStart < existingEnd && newEnd > existingStart)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

// Enhanced conflict detection with detailed information
static int checkRentalConflicts(Rental *head, int vehicleId, time_t newStart, time_t newEnd, char *conflictInfo, size_t infoSize)
{
    int conflictCount = 0;
    char tempInfo[256];
    
    for (Rental *r = head; r; r = r->next)
    {
        if (r->vehicleId == vehicleId && r->status == RENT_ACTIVE)
        {
            time_t existingStart, existingEnd;

            if (stringToTime(r->startTime, &existingStart) && stringToTime(r->endTime, &existingEnd))
            {
                // Check for overlap
                if (newStart < existingEnd && newEnd > existingStart)
                {
                    conflictCount++;
                    
                    // Format conflict information
                    snprintf(tempInfo, sizeof(tempInfo), 
                            "Conflict #%d: Rental ID %d (Customer %d) - %s to %s\n",
                            conflictCount, r->id, r->customerId, r->startTime, r->endTime);
                    
                    if (conflictInfo && infoSize > 0)
                    {
                        strncat(conflictInfo, tempInfo, infoSize - strlen(conflictInfo) - 1);
                    }
                }
            }
        }
    }
    
    return conflictCount;
}



// Public function to check vehicle availability for a specific time range
int isVehicleAvailableForTime(Rental *head, int vehicleId, time_t startTime, time_t endTime)
{
    return !isVehicleBooked(head, vehicleId, startTime, endTime);
}

// Public function to validate rental time range
int validateRentalTimeRange(time_t startTime, time_t endTime, int rentalType)
{
    time_t now = time(NULL);
    time_t minDuration, maxDuration;
    
    // Check if start time is in the future
    if (startTime <= now)
    {
        return 0;
    }
    
    // Check if end time is after start time
    if (endTime <= startTime)
    {
        return 0;
    }
    
    // Validate duration based on rental type
    time_t duration = endTime - startTime;
    
    switch (rentalType)
    {
        case RENT_HOURLY:
            minDuration = 60 * 60;        // 1 hour minimum
            maxDuration = 24 * 60 * 60;   // 24 hours maximum
            break;
        case RENT_DAILY:
            minDuration = 24 * 60 * 60;   // 1 day minimum
            maxDuration = 30 * 24 * 60 * 60; // 30 days maximum
            break;
        case RENT_ROUTE:
            minDuration = 5 * 60;         // 5 minutes minimum
            maxDuration = 24 * 60 * 60;   // 24 hours maximum
            break;
        default:
            return 0;
    }
    
    if (duration < minDuration || duration > maxDuration)
    {
        return 0;
    }
    
    return 1;
}

static int nextRentalId = 5001;

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

static Rental *parseRentalCSV(char *line)
{
    Rental *r = (Rental *)malloc(sizeof(Rental));
    if (!r)
        return NULL;
    r->next = NULL;

    int id, custId, vehId, routeId, driverId, type, status, vehicleRating, driverRating;
    char start[32] = {0}, end[32] = {0}, comment[51] = {0};
    float total;

    int n = sscanf(line, "%d,%d,%d,%d,%d,%d,%19[^,],%19[^,],%f,%d,%d,%d,%50[^,\n]",
                   &id, &custId, &vehId, &routeId, &driverId, &type, start, end, &total, &status, &vehicleRating, &driverRating, comment);

    if (n == 10)
    {
        vehicleRating = 0;
        driverRating = 0;
        comment[0] = '\0';
    }
    else if (n == 12)
    {
        comment[0] = '\0';
    }
    else if (n != 13)
    {
        free(r);
        return NULL;
    }

    r->id = id;
    r->customerId = custId;
    r->vehicleId = vehId;
    r->routeId = routeId;
    r->driverId = driverId;
    r->type = (RentalType)type;
    strncpy(r->startTime, start, sizeof(r->startTime) - 1);
    r->startTime[sizeof(r->startTime) - 1] = '\0';
    strncpy(r->endTime, end, sizeof(r->endTime) - 1);
    r->endTime[sizeof(r->endTime) - 1] = '\0';
    r->totalCost = total;
    r->status = (RentalStatus)status;
    r->vehicleRating = vehicleRating;
    r->driverRating = driverRating;
    strncpy(r->comment, comment, sizeof(r->comment) - 1);
    r->comment[sizeof(r->comment) - 1] = '\0';

    return r;
}

void loadRentals(Rental **head)
{
    *head = NULL;
    FILE *f = fopen(RENTAL_FILE, "r");
    if (!f)
        return;

    char line[512];
    if (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "id,", 3) != 0)
        {
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
    fprintf(f, "id,customerId,vehicleId,routeId,driverId,type,startTime,endTime,totalCost,status,vehicleRating,driverRating,comment\n");
    for (Rental *r = head; r; r = r->next)
    {
        fprintf(f, "%d,%d,%d,%d,%d,%d,%s,%s,%.2f,%d,%d,%d,%s\n",
                r->id, r->customerId, r->vehicleId, r->routeId, r->driverId, (int)r->type,
                r->startTime, r->endTime, r->totalCost, (int)r->status, r->vehicleRating, r->driverRating, r->comment);
    }
    fclose(f);
}

Rental *findRentalById(Rental *head, int rentalId)
{
    for (Rental *cur = head; cur; cur = cur->next)
        if (cur->id == rentalId)
            return cur;
    return NULL;
}

int completeRental(Rental *r, Vehicle *vehicleHead, Driver *driverHead)
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

    Vehicle *v = findVehicleById(vehicleHead, r->vehicleId);
    if (v)
    {
        v->available = 1;
    }
    else
    {
        printf("Warning: vehicle #%d not found; continue anyway.\n", r->vehicleId);
    }

    char choice[10];
    getStringInput("\nWould you like to rate this rental experience? (y/n): ", choice, 10);

    if (choice[0] == 'y' || choice[0] == 'Y')
    {
        printf("\n--- Rate Your Experience (1-5 Stars) ---\n");
        printf("1 = Poor, 2 = Fair, 3 = Good, 4 = Very Good, 5 = Excellent\n");

        int v_rating = getIntegerInput("How was the vehicle? ", 1, 5);
        int d_rating = getIntegerInput("How was the driver? ", 1, 5);

        r->vehicleRating = v_rating;
        r->driverRating = d_rating;

        printf("\nQuick comment (optional, max 50 characters): ");
        char tempComment[51];
        getStringInput("", tempComment, 51);

        if (strlen(tempComment) > 0)
        {
            strncpy(r->comment, tempComment, sizeof(r->comment) - 1);
            r->comment[sizeof(r->comment) - 1] = '\0';
        }
        else
        {
            r->comment[0] = '\0';
        }

        if (v)
        {
            updateVehicleRating(vehicleHead, r->vehicleId, v_rating);
        }

        if (r->driverId > 0 && driverHead)
        {
            Driver *driver = findDriverById(driverHead, r->driverId);
            if (driver)
            {
                updateDriverRating(driver, (float)d_rating);
            }
        }

        printf("Thank you for your feedback!\n");
    }

    if (v)
    {
        saveVehicles(vehicleHead);
    }

    printf("Rental #%d marked COMPLETED. Vehicle #%d is now AVAILABLE.\n",
           r->id, r->vehicleId);

    if (r->driverId > 0 && driverHead)
    {
        Driver *driver = findDriverById(driverHead, r->driverId);
        if (driver)
        {
            completeDriverTrip(driver, r->totalCost * 0.3);
            printf("Driver #%d (%s) completed trip and is now AVAILABLE.\n",
                   driver->id, driver->name);
        }
    }

    return 1;
}

int cancelRental(Rental *r, Vehicle *vehicleHead, Driver *driverHead)
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

    if (r->driverId > 0 && driverHead)
    {
        Driver *driver = findDriverById(driverHead, r->driverId);
        if (driver)
        {
            updateDriverStatus(driver, DRIVER_AVAILABLE);
            printf("Driver #%d (%s) is now AVAILABLE.\n", driver->id, driver->name);
        }
    }

    return 1;
}

void completeRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead, Driver *driverHead)
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

    if (completeRental(r, vehicleHead, driverHead))
    {
        saveRentals(rentalHead);
        saveDrivers(driverHead);
    }
}

void cancelRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead, Driver *driverHead)
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

    if (cancelRental(r, vehicleHead, driverHead))
    {
        saveRentals(rentalHead);
        saveDrivers(driverHead);
    }
}

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

void displayRental(const Rental *r)
{
    if (!r)
        return;
    printf("Rental ID: %d | Customer ID: %d | Vehicle ID: %d | Driver ID: %d | Type: %d | Start: %s | End: %s | Cost: %.2f | Status: %d\n",
           r->id, r->customerId, r->vehicleId, r->driverId, r->type, r->startTime, r->endTime, r->totalCost, r->status);

    if (r->vehicleRating > 0 || r->driverRating > 0)
    {
        printf("   Ratings - Vehicle: %d/5, Driver: %d/5", r->vehicleRating, r->driverRating);
        if (r->comment[0] != '\0')
        {
            printf(" | Comment: \"%s\"", r->comment);
        }
        printf("\n");
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

void createRentalByCustomer(Rental **rentalHead, Vehicle *vehicleHead, Customer *current, Promo *promoHead, Driver *driverHead, Invoice **invoiceHead)
{
    if (!current)
    {
        printf("Error: no current customer.\n");
        return;
    }

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
    r->driverId = 0;

    nowString(r->startTime, sizeof(r->startTime));

    if (tchoice == 1)
    {
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

    char promo_choice[10];
    getStringInput("\nDo you have a promo code? (y/n): ", promo_choice, 10);

    if (promo_choice[0] == 'y' || promo_choice[0] == 'Y')
    {
        char code_input[20];
        getStringInput("Enter your promo code: ", code_input, 20);

        Promo *promo = findActivePromoByCode(promoHead, code_input);

        if (promo)
        {
            float original_cost = r->totalCost;
            float discount_amount = original_cost * (promo->discountPercent / 100.0);
            r->totalCost = original_cost - discount_amount;

            printf("\nSuccess! Promo code '%s' applied.\n", promo->code);
            printf("  Original Price: $%.2f\n", original_cost);
            printf("  Discount (%.1f%%): -$%.2f\n", promo->discountPercent, discount_amount);
            printf("  New Final Price:  $%.2f\n", r->totalCost);
        }
        else
        {
            printf("Sorry, that promo code is invalid or has been deactivated.\n");
        }
    }

    if (driverHead)
    {
        Vehicle *vehicle = findVehicleById(vehicleHead, v->id);
        if (vehicle)
        {
            Driver *assignedDriver = assignDriverToRental(driverHead, vehicleTypeStr(vehicle->type));
            if (assignedDriver)
            {
                r->driverId = assignedDriver->id;
                printf("\n--- DRIVER ASSIGNED ---\n");
                printf("Driver: %s (ID: %d)\n", assignedDriver->name, assignedDriver->id);
                printf("Phone: %s | Rating: %.2f/5.0\n", assignedDriver->phone, assignedDriver->rating);
                printf("Total Trips: %d | Total Earnings: $%d\n", assignedDriver->totalTrips, assignedDriver->totalEarnings);
            }
            else
            {
                printf("\n--- NO DRIVER AVAILABLE ---\n");
                printf("No available drivers for %s type vehicles.\n", vehicleTypeStr(vehicle->type));
                printf("Rental will proceed without driver assignment.\n");
            }
        }
    }

    time_t newStartTime, newEndTime;
    if (stringToTime(r->startTime, &newStartTime) && stringToTime(r->endTime, &newEndTime))
    {
        // Validate rental time range
        if (!validateRentalTimeRange(newStartTime, newEndTime, r->type))
        {
            printf("\n--- BOOKING FAILED ---\n");
            printf("Invalid rental time range.\n");
            free(r);
            return;
        }
        
        // Check for conflicts with detailed information
        char conflictInfo[1024] = "";
        int conflictCount = checkRentalConflicts(*rentalHead, v->id, newStartTime, newEndTime, conflictInfo, sizeof(conflictInfo));
        
        if (conflictCount > 0)
        {
            printf("\n--- BOOKING FAILED ---\n");
            printf("Sorry, vehicle #%d is already booked during the requested time.\n", v->id);
            printf("Conflicts found: %d\n", conflictCount);
            printf("\n--- CONFLICT DETAILS ---\n");
            printf("%s", conflictInfo);
            printf("\nPlease try different dates or another vehicle.\n");
            free(r);
            return;
        }
        
        printf("\n--- CONFLICT CHECK PASSED ---\n");
        printf("No conflicts found. Vehicle is available for the requested time.\n");
    }
    else
    {
        printf("Error: Invalid date format entered.\n");
        free(r);
        return;
    }

    r->next = *rentalHead;
    *rentalHead = r;

    v->available = 0;
    saveVehicles(vehicleHead);
    saveRentals(*rentalHead);

    if (invoiceHead)
    {
        float originalCost = r->totalCost;
        float discountAmount = 0.0;
        char promoCodeUsed[20] = "";

        if (r->totalCost < originalCost)
        {
            discountAmount = originalCost - r->totalCost;
            for (Promo *p = promoHead; p; p = p->next)
            {
                if (p->isActive && p->discountPercent > 0)
                {
                    float expectedDiscount = originalCost * (p->discountPercent / 100.0);
                    if (fabs(expectedDiscount - discountAmount) < 0.01)
                    {
                        strncpy(promoCodeUsed, p->code, sizeof(promoCodeUsed) - 1);
                        promoCodeUsed[sizeof(promoCodeUsed) - 1] = '\0';
                        break;
                    }
                }
            }
        }

        Invoice *invoice = createInvoice(r->id, r->customerId, r->driverId,
                                         originalCost, discountAmount, promoCodeUsed);
        if (invoice)
        {
            invoice->next = *invoiceHead;
            *invoiceHead = invoice;

            Vehicle *vehicle = findVehicleById(vehicleHead, r->vehicleId);
            char vehicleInfo[256];
            if (vehicle)
            {
                snprintf(vehicleInfo, sizeof(vehicleInfo), "%s %s %s", vehicle->make, vehicle->model, vehicleTypeStr(vehicle->type));
            }
            else
            {
                strncpy(vehicleInfo, "Unknown Vehicle", sizeof(vehicleInfo) - 1);
                vehicleInfo[sizeof(vehicleInfo) - 1] = '\0';
            }

            Driver *driver = NULL;
            if (r->driverId > 0)
            {
                driver = findDriverById(driverHead, r->driverId);
            }

            generateReceipt(invoice, current->name, vehicleInfo,
                            driver ? driver->name : NULL);
            saveReceiptToFile(invoice, current->name, vehicleInfo,
                              driver ? driver->name : NULL);
        }
    }

    printf("\nRental created!\n");
    printf("Rental ID: %d | Vehicle: %d | Type: %s | Start: %s | End: %s | Cost: $%.2f | Status: %s\n",
           r->id, r->vehicleId, typeStr(r->type), r->startTime, r->endTime, r->totalCost, statusStr(r->status));
}

void listAllRentals(Rental *head)
{
    if (!head)
    {
        printf("No rentals found.\n");
        return;
    }

    printf("\n=== All Rentals ===\n");
    for (Rental *r = head; r; r = r->next)
    {
        displayRental(r);
    }
}

void freeRentalList(Rental **head)
{
    Rental *current = *head;
    Rental *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}

void displayVehicleReviews(Rental *rentalHead, int vehicleId)
{
    printf("\n--- Vehicle Reviews ---\n");
    int reviewCount = 0;

    for (Rental *r = rentalHead; r; r = r->next)
    {
        if (r->vehicleId == vehicleId && r->status == RENT_COMPLETED &&
            (r->vehicleRating > 0 || r->driverRating > 0))
        {

            printf("★★★★★ ");
            for (int i = 0; i < r->vehicleRating; i++)
                printf("★");
            for (int i = r->vehicleRating; i < 5; i++)
                printf("☆");
            printf(" ");

            if (r->comment[0] != '\0')
            {
                printf("\"%s\"", r->comment);
            }
            else
            {
                printf("(No comment)");
            }

            printf(" (Rental #%d)\n", r->id);
            reviewCount++;
        }
    }

    if (reviewCount == 0)
    {
        printf("No reviews yet for this vehicle.\n");
    }
    else
    {
        printf("\nTotal reviews: %d\n", reviewCount);
    }
}

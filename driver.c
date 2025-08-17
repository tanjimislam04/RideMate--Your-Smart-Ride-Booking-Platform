// File: driver.c
// Description: Implements driver management system for RideMate

#include "driver.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DRIVER_FILE "data/drivers.csv"

// --- Helper Functions ---

static void ensureDriverFileExists()
{
    FILE *f = fopen(DRIVER_FILE, "r");
    if (f)
    {
        fclose(f);
        return; // File already exists
    }

    f = fopen(DRIVER_FILE, "w");
    if (f)
    {
        fprintf(f, "id,name,phone,licenseNumber,vehicleType,rating,totalTrips,totalEarnings,status,lastActive\n");
        fclose(f);
    }
}

static Driver *parseDriverCSV(char *line)
{
    Driver *d = (Driver *)malloc(sizeof(Driver));
    if (!d)
        return NULL;

    d->next = NULL;

    int id, status, totalTrips, totalEarnings;
    char name[50], phone[15], licenseNumber[20], vehicleType[20];
    float rating;
    long lastActive;

    int n = sscanf(line, "%d,%49[^,],%14[^,],%19[^,],%19[^,],%f,%d,%d,%d,%ld",
                   &id, name, phone, licenseNumber, vehicleType,
                   &rating, &totalTrips, &totalEarnings, &status, &lastActive);

    if (n != 10)
    {
        free(d);
        return NULL;
    }

    d->id = id;
    strncpy(d->name, name, sizeof(d->name) - 1);
    d->name[sizeof(d->name) - 1] = '\0';
    strncpy(d->phone, phone, sizeof(d->phone) - 1);
    d->phone[sizeof(d->phone) - 1] = '\0';
    strncpy(d->licenseNumber, licenseNumber, sizeof(d->licenseNumber) - 1);
    d->licenseNumber[sizeof(d->licenseNumber) - 1] = '\0';
    strncpy(d->vehicleType, vehicleType, sizeof(d->vehicleType) - 1);
    d->vehicleType[sizeof(d->vehicleType) - 1] = '\0';
    d->rating = rating;
    d->totalTrips = totalTrips;
    d->totalEarnings = totalEarnings;
    d->status = (DriverStatus)status;
    d->lastActive = (time_t)lastActive;

    return d;
}

static int getNextDriverId(Driver *head)
{
    int maxId = 0;
    for (Driver *d = head; d; d = d->next)
    {
        if (d->id > maxId)
            maxId = d->id;
    }
    return maxId + 1;
}

// --- CSV I/O Functions ---

void loadDrivers(Driver **head)
{
    *head = NULL;
    ensureDriverFileExists();

    FILE *f = fopen(DRIVER_FILE, "r");
    if (!f)
        return;

    char line[512];
    // Skip header
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

        Driver *d = parseDriverCSV(line);
        if (d)
        {
            d->next = *head;
            *head = d;
        }
    }
    fclose(f);
}

void saveDrivers(Driver *head)
{
    FILE *f = fopen(DRIVER_FILE, "w");
    if (!f)
    {
        printf("Error: could not open %s\n", DRIVER_FILE);
        return;
    }

    fprintf(f, "id,name,phone,licenseNumber,vehicleType,rating,totalTrips,totalEarnings,status,lastActive\n");
    for (Driver *d = head; d; d = d->next)
    {
        fprintf(f, "%d,%s,%s,%s,%s,%.2f,%d,%d,%d,%ld\n",
                d->id, d->name, d->phone, d->licenseNumber, d->vehicleType,
                d->rating, d->totalTrips, d->totalEarnings, (int)d->status, (long)d->lastActive);
    }
    fclose(f);
}

// --- Driver Management Functions ---

Driver *findDriverById(Driver *head, int driverId)
{
    for (Driver *d = head; d; d = d->next)
    {
        if (d->id == driverId)
            return d;
    }
    return NULL;
}

Driver *findAvailableDriver(Driver *head, const char *vehicleType)
{
    Driver *bestDriver = NULL;
    float bestRating = 0.0;

    for (Driver *d = head; d; d = d->next)
    {
        if (d->status == DRIVER_AVAILABLE &&
            strcmp(d->vehicleType, vehicleType) == 0 &&
            d->rating > bestRating)
        {
            bestDriver = d;
            bestRating = d->rating;
        }
    }
    return bestDriver;
}

void addDriver(Driver **head)
{
    clearScreen();
    printf("\n--- Add New Driver ---\n");

    Driver *d = (Driver *)malloc(sizeof(Driver));
    if (!d)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    d->id = getNextDriverId(*head);
    d->rating = 0.0;
    d->totalTrips = 0;
    d->totalEarnings = 0;
    d->status = DRIVER_AVAILABLE;
    d->lastActive = time(NULL);

    getStringInput("Enter driver name: ", d->name, sizeof(d->name));
    getStringInput("Enter phone number: ", d->phone, sizeof(d->phone));
    getStringInput("Enter license number: ", d->licenseNumber, sizeof(d->licenseNumber));

    printf("\nSelect vehicle type:\n");
    printf("1. Car\n");
    printf("2. SUV\n");
    printf("3. Motorcycle\n");
    printf("4. Van\n");
    int choice = getIntegerInput("Choose (1-4): ", 1, 4);

    switch (choice)
    {
    case 1:
        strcpy(d->vehicleType, "Car");
        break;
    case 2:
        strcpy(d->vehicleType, "SUV");
        break;
    case 3:
        strcpy(d->vehicleType, "Motorcycle");
        break;
    case 4:
        strcpy(d->vehicleType, "Van");
        break;
    }

    d->next = *head;
    *head = d;
    saveDrivers(*head);

    printf("\nDriver added successfully! ID: %d\n", d->id);
}

void updateDriverProfile(Driver *driver)
{
    if (!driver)
        return;

    clearScreen();
    printf("\n--- Update Driver Profile (ID: %d) ---\n", driver->id);

    printf("Current name: %s\n", driver->name);
    getStringInput("New name (or press Enter to keep current): ", driver->name, sizeof(driver->name));

    printf("Current phone: %s\n", driver->phone);
    getStringInput("New phone (or press Enter to keep current): ", driver->phone, sizeof(driver->phone));

    printf("Current license: %s\n", driver->licenseNumber);
    getStringInput("New license (or press Enter to keep current): ", driver->licenseNumber, sizeof(driver->licenseNumber));

    printf("Current vehicle type: %s\n", driver->vehicleType);
    printf("New vehicle type:\n");
    printf("1. Car\n");
    printf("2. SUV\n");
    printf("3. Motorcycle\n");
    printf("4. Van\n");
    printf("5. Keep current\n");
    int choice = getIntegerInput("Choose (1-5): ", 1, 5);

    if (choice <= 4)
    {
        switch (choice)
        {
        case 1:
            strcpy(driver->vehicleType, "Car");
            break;
        case 2:
            strcpy(driver->vehicleType, "SUV");
            break;
        case 3:
            strcpy(driver->vehicleType, "Motorcycle");
            break;
        case 4:
            strcpy(driver->vehicleType, "Van");
            break;
        }
    }

    driver->lastActive = time(NULL);
    printf("\nDriver profile updated successfully!\n");
}

void displayDriverProfile(const Driver *driver)
{
    if (!driver)
        return;

    printf("\n--- Driver Profile ---\n");
    printf("ID: %d\n", driver->id);
    printf("Name: %s\n", driver->name);
    printf("Phone: %s\n", driver->phone);
    printf("License: %s\n", driver->licenseNumber);
    printf("Vehicle Type: %s\n", driver->vehicleType);
    printf("Rating: %.2f/5.0\n", driver->rating);
    printf("Total Trips: %d\n", driver->totalTrips);
    printf("Total Earnings: $%d\n", driver->totalEarnings);
    printf("Status: %s\n",
           driver->status == DRIVER_AVAILABLE ? "Available" : driver->status == DRIVER_BUSY ? "Busy"
                                                                                            : "Offline");

    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M", localtime(&driver->lastActive));
    printf("Last Active: %s\n", timeStr);
}

void listAllDrivers(Driver *head)
{
    if (!head)
    {
        printf("No drivers found.\n");
        return;
    }

    printf("\n--- All Drivers ---\n");
    printf("%-4s %-20s %-15s %-12s %-10s %-6s %-8s %-8s %-10s\n",
           "ID", "Name", "Phone", "License", "Vehicle", "Rating", "Trips", "Earnings", "Status");
    printf("--------------------------------------------------------------------------------\n");

    for (Driver *d = head; d; d = d->next)
    {
        printf("%-4d %-20s %-15s %-12s %-10s %-6.2f %-8d %-8d %-10s\n",
               d->id, d->name, d->phone, d->licenseNumber, d->vehicleType,
               d->rating, d->totalTrips, d->totalEarnings,
               d->status == DRIVER_AVAILABLE ? "Available" : d->status == DRIVER_BUSY ? "Busy"
                                                                                      : "Offline");
    }
}

void listAvailableDrivers(Driver *head)
{
    if (!head)
    {
        printf("No drivers found.\n");
        return;
    }

    printf("\n--- Available Drivers ---\n");
    printf("%-4s %-20s %-10s %-6s %-8s %-8s\n",
           "ID", "Name", "Vehicle", "Rating", "Trips", "Earnings");
    printf("------------------------------------------------\n");

    int found = 0;
    for (Driver *d = head; d; d = d->next)
    {
        if (d->status == DRIVER_AVAILABLE)
        {
            printf("%-4d %-20s %-10s %-6.2f %-8d %-8d\n",
                   d->id, d->name, d->vehicleType, d->rating, d->totalTrips, d->totalEarnings);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No available drivers.\n");
    }
}

// --- Driver Assignment Functions ---

Driver *assignDriverToRental(Driver *head, const char *vehicleType)
{
    Driver *driver = findAvailableDriver(head, vehicleType);
    if (driver)
    {
        updateDriverStatus(driver, DRIVER_BUSY);
        driver->lastActive = time(NULL);
        return driver;
    }
    return NULL;
}

void updateDriverStatus(Driver *driver, DriverStatus status)
{
    if (driver)
    {
        driver->status = status;
        driver->lastActive = time(NULL);
    }
}

void updateDriverRating(Driver *driver, float newRating)
{
    if (driver && newRating >= 0.0 && newRating <= 5.0)
    {
        // Calculate new average rating
        float totalRating = driver->rating * driver->totalTrips + newRating;
        driver->totalTrips++;
        driver->rating = totalRating / driver->totalTrips;
        driver->lastActive = time(NULL);
    }
}

void completeDriverTrip(Driver *driver, float tripEarnings)
{
    if (driver)
    {
        driver->totalTrips++;
        driver->totalEarnings += (int)tripEarnings;
        driver->status = DRIVER_AVAILABLE;
        driver->lastActive = time(NULL);
    }
}

// --- Driver Statistics ---

void showDriverStatistics(Driver *head)
{
    if (!head)
    {
        printf("No drivers found.\n");
        return;
    }

    int totalDrivers = 0, availableDrivers = 0, busyDrivers = 0, offlineDrivers = 0;
    float totalRating = 0.0, totalEarnings = 0.0, totalTrips = 0.0;

    for (Driver *d = head; d; d = d->next)
    {
        totalDrivers++;
        totalRating += d->rating;
        totalEarnings += d->totalEarnings;
        totalTrips += d->totalTrips;

        switch (d->status)
        {
        case DRIVER_AVAILABLE:
            availableDrivers++;
            break;
        case DRIVER_BUSY:
            busyDrivers++;
            break;
        case DRIVER_OFFLINE:
            offlineDrivers++;
            break;
        }
    }

    printf("\n--- Driver Statistics ---\n");
    printf("Total Drivers: %d\n", totalDrivers);
    printf("Available: %d | Busy: %d | Offline: %d\n", availableDrivers, busyDrivers, offlineDrivers);
    printf("Average Rating: %.2f/5.0\n", totalDrivers > 0 ? totalRating / totalDrivers : 0.0);
    printf("Total Trips: %.0f\n", totalTrips);
    printf("Total Earnings: $%.0f\n", totalEarnings);
}

void showTopDrivers(Driver *head, int count)
{
    if (!head)
    {
        printf("No drivers found.\n");
        return;
    }

    // Create a temporary array to sort drivers
    int driverCount = 0;
    for (Driver *d = head; d; d = d->next)
        driverCount++;

    Driver **drivers = (Driver **)malloc(driverCount * sizeof(Driver *));
    if (!drivers)
        return;

    int i = 0;
    for (Driver *d = head; d; d = d->next)
    {
        drivers[i++] = d;
    }

    // Sort by rating (bubble sort for simplicity)
    for (int i = 0; i < driverCount - 1; i++)
    {
        for (int j = 0; j < driverCount - i - 1; j++)
        {
            if (drivers[j]->rating < drivers[j + 1]->rating)
            {
                Driver *temp = drivers[j];
                drivers[j] = drivers[j + 1];
                drivers[j + 1] = temp;
            }
        }
    }

    printf("\n--- Top %d Drivers by Rating ---\n", count > driverCount ? driverCount : count);
    printf("%-4s %-20s %-10s %-6s %-8s %-8s\n",
           "ID", "Name", "Vehicle", "Rating", "Trips", "Earnings");
    printf("------------------------------------------------\n");

    for (int i = 0; i < count && i < driverCount; i++)
    {
        Driver *d = drivers[i];
        printf("%-4d %-20s %-10s %-6.2f %-8d %-8d\n",
               d->id, d->name, d->vehicleType, d->rating, d->totalTrips, d->totalEarnings);
    }

    free(drivers);
}

// --- Memory Management ---

void freeDriverList(Driver **head)
{
    Driver *current = *head;
    Driver *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}

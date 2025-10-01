#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "vehicle.h"
#include "rental.h"

#define VEHICLE_FILE "data/vehicles.csv"
#define ROUTE_FILE "data/routes.csv"

extern Route *routeHead;

static int nextVehicleId = 2001;
static int nextRouteId = 3001;

static void displayStarRating(float rating);

static void ensureCsvWithHeader(const char *path, const char *headerLine)
{
    FILE *r = fopen(path, "r");
    if (r)
    {
        fclose(r);
        return;
    }
    FILE *w = fopen(path, "w");
    if (!w)
    {
        printf("Error: could not create %s\n", path);
        perror("File creation error");
        return;
    }
    fputs(headerLine, w);
    fputc('\n', w);
    fclose(w);
    printf("Created new CSV file: %s\n", path);
}

static void adjustVehicleNextId(Vehicle *head)
{
    int maxId = 2000;
    for (Vehicle *v = head; v; v = v->next)
        if (v->id > maxId)
            maxId = v->id;
    nextVehicleId = maxId + 1;
}

static void adjustRouteNextId(Route *head)
{
    int maxId = 3000;
    for (Route *r = head; r; r = r->next)
        if (r->id > maxId)
            maxId = r->id;
    nextRouteId = maxId + 1;
}

static Vehicle *parseVehicleCSV(char *line)
{
    Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
    if (!v)
        return NULL;
    
    int result = sscanf(line, "%d,%49[^,],%49[^,],%d,%d,%f,%f,%d,%d,%d,%f",
           &v->id, v->make, v->model, &v->year, (int *)&v->type, &v->ratePerDay, &v->ratePerHour, &v->active, &v->available, &v->ratingCount, &v->averageRating);
    
    if (result != 11) {
        printf("Warning: Failed to parse vehicle line: %s (parsed %d fields)\n", line, result);
        free(v);
        return NULL;
    }
    
    v->next = NULL;
    return v;
}

static Route *parseRouteCSV(char *line)
{
    Route *r = (Route *)malloc(sizeof(Route));
    if (!r)
        return NULL;
    sscanf(line, "%d,%49[^,],%49[^,],%49[^,],%f,%d,%d",
           &r->id, r->name, r->from, r->to, &r->baseFare, &r->etaMin, &r->active);
    r->next = NULL;
    return r;
}

void loadVehicles(Vehicle **head)
{
    ensureCsvWithHeader(VEHICLE_FILE, "id,make,model,year,type,ratePerDay,ratePerHour,active,available,ratingCount,averageRating\n");
    FILE *f = fopen(VEHICLE_FILE, "r");
    if (!f)
    {
        printf("Warning: Could not open %s for reading\n", VEHICLE_FILE);
        perror("File error");
        *head = NULL;
        return;
    }
    char line[256];
    fgets(line, sizeof(line), f); // Skip header
    int count = 0;
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        Vehicle *v = parseVehicleCSV(line);
        if (v)
        {
            v->next = *head;
            *head = v;
            count++;
        }
    }
    fclose(f);
    adjustVehicleNextId(*head);
    printf("Loaded %d vehicles from %s\n", count, VEHICLE_FILE);
}

void saveVehicles(Vehicle *head)
{
    FILE *f = fopen(VEHICLE_FILE, "w");
    if (!f)
    {
        printf("Error: Could not open file %s for writing\n", VEHICLE_FILE);
        perror("File error");
        return;
    }
    
    fprintf(f, "id,make,model,year,type,ratePerDay,ratePerHour,active,available,ratingCount,averageRating\n");
    int count = 0;
    for (Vehicle *v = head; v; v = v->next)
    {
        fprintf(f, "%d,%s,%s,%d,%d,%.2f,%.2f,%d,%d,%d,%.2f\n", 
                v->id, v->make, v->model, v->year, v->type, 
                v->ratePerDay, v->ratePerHour, v->active, v->available, 
                v->ratingCount, v->averageRating);
        count++;
    }
    fclose(f);
    printf("Successfully saved %d vehicles to %s\n", count, VEHICLE_FILE);
}

void loadRoutes(Route **head)
{
    ensureCsvWithHeader(ROUTE_FILE, "id,name,from,to,baseFare,etaMin,active\n");
    FILE *f = fopen(ROUTE_FILE, "r");
    if (!f)
    {
        *head = NULL;
        return;
    }
    char line[256];
    fgets(line, sizeof(line), f);
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        Route *r = parseRouteCSV(line);
        if (r)
        {
            r->next = *head;
            *head = r;
        }
    }
    fclose(f);
    routeHead = *head;
    adjustRouteNextId(*head);
}

void saveRoutes(Route *head)
{
    FILE *f = fopen(ROUTE_FILE, "w");
    if (!f)
        return;
    fprintf(f, "id,name,from,to,baseFare,etaMin,active\n");
    for (Route *r = head; r; r = r->next)
    {
        fprintf(f, "%d,%s,%s,%s,%.2f,%d,%d\n", r->id, r->name, r->from, r->to, r->baseFare, r->etaMin, r->active);
    }
    fclose(f);
    routeHead = head;
}

Vehicle *findVehicleById(Vehicle *head, int id)
{
    for (Vehicle *v = head; v; v = v->next)
        if (v->id == id)
            return v;
    return NULL;
}

Route *findRouteById(Route *head, int id)
{
    for (Route *r = head; r; r = r->next)
        if (r->id == id)
            return r;
    return NULL;
}

const char *vehicleTypeStr(VehicleType t)
{
    switch (t)
    {
    case VTYPE_CAR:
        return "Car";
    case VTYPE_MOTORCYCLE:
        return "Motorcycle";
    case VTYPE_TRUCK:
        return "Truck";
    case VTYPE_VAN:
        return "Van";
    default:
        return "Unknown";
    }
}

void displayVehicle(const Vehicle *v)
{
    if (!v)
        return;
    printf("ID: %-5d | %s %s (%d) | Type: %-10s | Rate: $%.2f/day, $%.2f/hr | Status: %s | Rating: ",
           v->id, v->make, v->model, v->year, vehicleTypeStr(v->type), v->ratePerDay, v->ratePerHour,
           v->available ? "Available" : "Rented");

    if (v->ratingCount > 0)
    {
        displayStarRating(v->averageRating);
        printf(" (%.1f/5.0, %d reviews)", v->averageRating, v->ratingCount);
    }
    else
    {
        printf("No ratings yet");
    }
    printf("\n");
}

void listAllVehicles(Vehicle *head)
{
    printf("\n--- All Vehicles (Active & Inactive) ---\n");
    for (Vehicle *v = head; v; v = v->next)
        displayVehicle(v);
}

void displayAvailableVehicles(Vehicle *head)
{
    printf("\n--- Available Vehicles for Rent ---\n");
    int found = 0;
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->active && v->available)
        {
            displayVehicle(v);
            found = 1;
        }
    }
    if (!found)
        printf("No vehicles are currently available.\n");
}

void displayVehicleAvailabilitySchedule(Vehicle *head, Rental *rentalHead)
{
    printf("\n--- Vehicle Availability Schedule ---\n");
    int found = 0;
    
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->active)
        {
            printf("\nVehicle #%d: %s %s (%s)\n", v->id, v->make, v->model, vehicleTypeStr(v->type));
            printf("Status: %s\n", v->available ? "Available" : "Currently Rented");
            
            // Show current rental if any
            int hasActiveRental = 0;
            for (Rental *r = rentalHead; r; r = r->next)
            {
                if (r->vehicleId == v->id && r->status == RENT_ACTIVE)
                {
                    printf("  Current Rental: ID %d (Customer %d) - %s to %s\n", 
                           r->id, r->customerId, r->startTime, r->endTime);
                    hasActiveRental = 1;
                    break;
                }
            }
            
            if (!hasActiveRental)
            {
                printf("  No active rentals\n");
            }
            
            found = 1;
        }
    }
    
    if (!found)
        printf("No vehicles found.\n");
}

void displayAllRoutes(Route *head)
{
    printf("\n--- All Available Routes ---\n");
    int found = 0;
    for (Route *r = head; r; r = r->next)
    {
        if (r->active)
        {
            printf("ID: %-5d | %s (%s -> %s) | Fare: $%.2f\n", r->id, r->name, r->from, r->to, r->baseFare);
            found = 1;
        }
    }
    if (!found)
        printf("No routes are currently configured.\n");
}

void addVehicleRating(Vehicle *v, int rating)
{
    if (!v || rating < 1 || rating > 5)
        return;

    float totalRating = v->averageRating * v->ratingCount + rating;
    v->ratingCount++;
    v->averageRating = totalRating / v->ratingCount;
}

static void displayStarRating(float rating)
{
    int fullStars = (int)rating;
    int hasHalfStar = (rating - fullStars) >= 0.5;

    for (int i = 0; i < fullStars; i++)
        printf("★");
    if (hasHalfStar)
        printf("☆");
    for (int i = fullStars + hasHalfStar; i < 5; i++)
        printf("☆");
}

static void addVehicleInteractive(Vehicle **head)
{
    Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
    if (!v) {
        printf("Error: Memory allocation failed\n");
        return;
    }
    
    v->id = nextVehicleId++;
    getStringInput("Enter Brand Name: ", v->make, MAX_STRING);
    getStringInput("Enter Model: ", v->model, MAX_STRING);
    v->year = getIntegerInput("Enter Year: ", 2000, 2025);
    printf("Types: 0=Car, 1=Motorcycle, 2=Truck, 3=Van\n");
    v->type = (VehicleType)getIntegerInput("Enter Type: ", 0, 3);
    v->ratePerDay = getFloatInput("Enter Rate per Day: ", 1.0, 500000.0);
    v->ratePerHour = getFloatInput("Enter Rate per Hour: ", 1.0, 5000.0);
    v->available = 1;
    v->active = 1;
    v->ratingCount = 0;
    v->averageRating = 0.0;
    v->next = *head;
    *head = v;
    
    printf("Adding vehicle to memory: ID=%d, Make=%s, Model=%s\n", v->id, v->make, v->model);
    saveVehicles(*head);
    printf("Vehicle #%d added successfully.\n", v->id);
}

static void updateVehicleInteractive(Vehicle *head)
{
    int id = getIntegerInput("Enter Vehicle ID to update: ", 2001, 9999);
    Vehicle *v = findVehicleById(head, id);
    if (!v)
    {
        printf("Vehicle not found.\n");
        return;
    }

    printf("Editing Vehicle #%d: %s %s\n", v->id, v->make, v->model);
    v->ratePerDay = getFloatInput("Enter new Rate per Day: ", 1.0, 20000.0);
    v->ratePerHour = getFloatInput("Enter new Rate per Hour: ", 1.0, 1000.0);
    v->available = getIntegerInput("Is it available? (1=Yes, 0=No): ", 0, 1);

    saveVehicles(head);
    printf("Vehicle updated successfully.\n");
}

static void softDeleteVehicleInteractive(Vehicle *head)
{
    int id = getIntegerInput("Enter Vehicle ID to toggle active status: ", 2001, 9999);
    Vehicle *v = findVehicleById(head, id);
    if (!v)
    {
        printf("Vehicle not found.\n");
        return;
    }

    v->active = !v->active;
    saveVehicles(head);
    printf("Vehicle #%d is now %s.\n", v->id, v->active ? "Active" : "Inactive");
}

static void addRouteInteractive(Route **head)
{
    Route *r = (Route *)malloc(sizeof(Route));
    r->id = nextRouteId++;
    getStringInput("Enter Route Name: ", r->name, MAX_STRING);
    getStringInput("From: ", r->from, MAX_STRING);
    getStringInput("To: ", r->to, MAX_STRING);
    r->baseFare = getFloatInput("Base Fare: ", 1.0, 10000.0);
    r->etaMin = getIntegerInput("Estimated Time (Minutes): ", 5, 1440);
    r->active = 1;
    r->next = *head;
    *head = r;
    saveRoutes(*head);
    printf("Route #%d added successfully.\n", r->id);
}

static void adminRouteMenu(Route **head)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Route Management ---\n");
        printf("1. List All Routes\n");
        printf("2. Add New Route\n");
        printf("3. Back\n");
        int choice = getIntegerInput("Choice: ", 1, 3);
        switch (choice)
        {
        case 1:
            displayAllRoutes(*head);
            break;
        case 2:
            addRouteInteractive(head);
            break;
        case 3:
            running = 0;
            break;
        }
        if (running)
            pressEnterToContinue();
    }
}

void adminVehicleMenu(Vehicle **head, Rental *rentalHead)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Vehicle & Route Management ---\n");
        printf("1. Add New Vehicle\n");
        printf("2. Update Vehicle Info\n");
        printf("3. List All Vehicles\n");
        printf("4. Toggle Vehicle Active Status (Soft Delete)\n");
        printf("5. Manage Routes\n");
        printf("6. View Top Rated Vehicles\n");
        printf("7. View Vehicle Availability Schedule\n");
        printf("8. Back to Main Admin Menu\n");
        int choice = getIntegerInput("Choice: ", 1, 8);
        switch (choice)
        {
        case 1:
            addVehicleInteractive(head);
            break;
        case 2:
            updateVehicleInteractive(*head);
            break;
        case 3:
            listAllVehicles(*head);
            break;
        case 4:
            softDeleteVehicleInteractive(*head);
            break;
        case 5:
            adminRouteMenu(&routeHead);
            break;
        case 6:
        {
            int count = getIntegerInput("Show top how many vehicles? (1-10): ", 1, 10);
            displayTopRatedVehicles(*head, count);
            pressEnterToContinue();
            break;
        }
        case 7:
            displayVehicleAvailabilitySchedule(*head, rentalHead);
            pressEnterToContinue();
            break;
        case 8:
            running = 0;
            break;
        }
        if (running)
            pressEnterToContinue();
    }
}

void freeVehicleList(Vehicle **head)
{
    Vehicle *current = *head;
    Vehicle *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}

void updateVehicleRating(Vehicle *head, int vehicleId, int newRating)
{
    Vehicle *v = findVehicleById(head, vehicleId);
    if (v && newRating >= 1 && newRating <= 5)
    {
        float totalRating = v->averageRating * v->ratingCount;
        v->ratingCount++;
        v->averageRating = (totalRating + newRating) / v->ratingCount;
    }
}

void displayTopRatedVehicles(Vehicle *head, int count)
{
    if (!head)
    {
        printf("No vehicles found.\n");
        return;
    }

    int totalVehicles = 0;
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->ratingCount > 0)
            totalVehicles++;
    }

    if (totalVehicles == 0)
    {
        printf("No vehicles have been rated yet.\n");
        return;
    }

    Vehicle **ratedVehicles = malloc(totalVehicles * sizeof(Vehicle *));
    if (!ratedVehicles)
        return;

    int index = 0;
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->ratingCount > 0)
        {
            ratedVehicles[index++] = v;
        }
    }

    for (int i = 0; i < totalVehicles - 1; i++)
    {
        for (int j = 0; j < totalVehicles - i - 1; j++)
        {
            if (ratedVehicles[j]->averageRating < ratedVehicles[j + 1]->averageRating)
            {
                Vehicle *temp = ratedVehicles[j];
                ratedVehicles[j] = ratedVehicles[j + 1];
                ratedVehicles[j + 1] = temp;
            }
        }
    }

    printf("\n--- Top %d Rated Vehicles ---\n", count > totalVehicles ? totalVehicles : count);
    printf("%-5s %-15s %-15s %-8s %-10s %-15s\n", "ID", "Make", "Model", "Type", "Rating", "Reviews");
    printf("----------------------------------------------------------------\n");

    for (int i = 0; i < count && i < totalVehicles; i++)
    {
        Vehicle *v = ratedVehicles[i];
        printf("%-5d %-15s %-15s %-8s ", v->id, v->make, v->model, vehicleTypeStr(v->type));
        displayStarRating(v->averageRating);
        printf(" (%.1f) %-5d\n", v->averageRating, v->ratingCount);
    }

    free(ratedVehicles);
}

void freeRouteList(Route **head)
{
    Route *current = *head;
    Route *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}
// File: vehicle.c (Corrected and Final Version)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "vehicle.h"

#define VEHICLE_FILE "data/vehicles.csv"
#define ROUTE_FILE "data/routes.csv"

// Global pointer, declared 'extern' in the header
Route *routeHead = NULL;

// Static ID counters for this module
static int nextVehicleId = 2001;
static int nextRouteId = 3001;

// --- Helper to create CSV files with headers if they don't exist ---
static void ensureCsvWithHeader(const char *path, const char *headerLine)
{
    FILE *r = fopen(path, "r");
    if (r)
    {
        fclose(r);
        return;
    } // File already exists
    FILE *w = fopen(path, "w");
    if (!w)
    {
        printf("Error: could not create %s\n", path);
        return;
    }
    fputs(headerLine, w);
    fputc('\n', w);
    fclose(w);
}

// --- ID Management ---
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

// --- CSV Parsing ---
static Vehicle *parseVehicleCSV(char *line)
{
    Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
    if (!v)
        return NULL;
    // CSV: id,make,model,year,type,ratePerDay,ratePerHour,active,available
    sscanf(line, "%d,%49[^,],%49[^,],%d,%d,%f,%f,%d,%d",
           &v->id, v->make, v->model, &v->year, (int *)&v->type, &v->ratePerDay, &v->ratePerHour, &v->active, &v->available);
    v->next = NULL;
    return v;
}

static Route *parseRouteCSV(char *line)
{
    Route *r = (Route *)malloc(sizeof(Route));
    if (!r)
        return NULL;
    // CSV: id,name,from,to,baseFare,etaMin,active
    sscanf(line, "%d,%49[^,],%49[^,],%49[^,],%f,%d,%d",
           &r->id, r->name, r->from, r->to, &r->baseFare, &r->etaMin, &r->active);
    r->next = NULL;
    return r;
}

// --- File I/O Functions ---
void loadVehicles(Vehicle **head)
{
    ensureCsvWithHeader(VEHICLE_FILE, "id,make,model,year,type,ratePerDay,ratePerHour,active,available\n");
    FILE *f = fopen(VEHICLE_FILE, "r");
    if (!f)
    {
        *head = NULL;
        return;
    }
    char line[256];
    fgets(line, sizeof(line), f); // Skip header
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        Vehicle *v = parseVehicleCSV(line);
        if (v)
        {
            v->next = *head;
            *head = v;
        }
    }
    fclose(f);
    adjustVehicleNextId(*head);
}

void saveVehicles(Vehicle *head)
{
    FILE *f = fopen(VEHICLE_FILE, "w");
    if (!f)
        return;
    fprintf(f, "id,make,model,year,type,ratePerDay,ratePerHour,active,available\n");
    for (Vehicle *v = head; v; v = v->next)
    {
        fprintf(f, "%d,%s,%s,%d,%d,%.2f,%.2f,%d,%d\n", v->id, v->make, v->model, v->year, v->type, v->ratePerDay, v->ratePerHour, v->active, v->available);
    }
    fclose(f);
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
    fgets(line, sizeof(line), f); // Skip header
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
    routeHead = *head; // Update the global head
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
    routeHead = head; // Update the global head
}

// --- Find Functions ---
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

// --- Display Functions ---
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
    printf("ID: %-5d | %s %s (%d) | Type: %-10s | Rate: $%.2f/day, $%.2f/hr | Status: %s\n",
           v->id, v->make, v->model, v->year, vehicleTypeStr(v->type), v->ratePerDay, v->ratePerHour,
           v->available ? "Available" : "Rented");
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

// --- Admin Helper Functions ---
static void addVehicleInteractive(Vehicle **head)
{
    Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
    v->id = nextVehicleId++;
    getStringInput("Enter Brand Name: ", v->make, MAX_STRING);
    getStringInput("Enter Model: ", v->model, MAX_STRING);
    v->year = getIntegerInput("Enter Year: ", 2000, 2025);
    printf("Types: 0=Car, 1=Motorcycle, 2=Truck, 3=Van\n");
    v->type = (VehicleType)getIntegerInput("Enter Type: ", 0, 3);
    v->ratePerDay = getFloatInput("Enter Rate per Day: ", 1.0, 20000.0);
    v->ratePerHour = getFloatInput("Enter Rate per Hour: ", 1.0, 1000.0);
    v->available = 1;
    v->active = 1;
    v->next = *head;
    *head = v;
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

    v->active = !v->active; // Flip the status
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

// --- Admin Menus ---
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
            listAllVehicles((Vehicle *)*head);
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

void adminVehicleMenu(Vehicle **head)
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
        printf("6. Back to Main Admin Menu\n");
        int choice = getIntegerInput("Choice: ", 1, 6);
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
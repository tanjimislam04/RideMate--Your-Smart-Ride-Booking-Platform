#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vehicle.h"
#include "utils.h"

#define VEHICLE_FILE "vehicles.csv"

Vehicle *parseVehicleCSV(char *line)
{
    Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
    if (!v)
        return NULL;
    v->next = NULL;
    // Format: id,model,type,available,price_per_hour,price_per_day,popularity,mileage,active
    sscanf(line, "%d,%49[^,],%29[^,],%d,%f,%f,%d,%d,%d",
           &v->id, v->model, v->type, &v->available, &v->price_per_hour, &v->price_per_day, &v->popularity, &v->mileage, &v->active);
    return v;
}

// Load all vehicles from CSV
void loadVehicles(Vehicle **head)
{
    FILE *f = fopen(VEHICLE_FILE, "r");
    if (!f)
    {
        *head = NULL;
        return;
    }
    char line[256];
    fgets(line, sizeof(line), f); // skip header
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
}

// Save all vehicles to CSV
void saveVehicles(Vehicle *head)
{
    FILE *f = fopen(VEHICLE_FILE, "w");
    if (!f)
        return;
    fprintf(f, "id,model,type,available,price_per_hour,price_per_day,popularity,mileage,active\n");
    for (Vehicle *v = head; v; v = v->next)
    {
        fprintf(f, "%d,%s,%s,%d,%.2f,%.2f,%d,%d,%d\n",
                v->id, v->model, v->type, v->available, v->price_per_hour, v->price_per_day,
                v->popularity, v->mileage, v->active);
    }
    fclose(f);
}

// Find a vehicle by its ID (and active)
Vehicle *findVehicleById(Vehicle *head, int id)
{
    for (Vehicle *v = head; v; v = v->next)
        if (v->id == id && v->active)
            return v;
    return NULL;
}

// Add a new vehicle (Admin)
void addVehicle(Vehicle **head)
{
    Vehicle *v = malloc(sizeof(Vehicle));
    v->id = generateUniqueId();
    v->active = 1;
    v->popularity = 0;

    getInput("Enter vehicle model: ", v->model, sizeof(v->model));
    getInput("Enter vehicle type (Car/Bike/Etc): ", v->type, sizeof(v->type));

    char buf[30];
    while (1)
    {
        getInput("Available (1=Yes, 0=No): ", buf, sizeof(buf));
        v->available = atoi(buf);
        if (v->available == 0 || v->available == 1)
            break;
        printf("Only 1 (Yes) or 0 (No).\n");
    }
    while (1)
    {
        getInput("Price per hour: ", buf, sizeof(buf));
        v->price_per_hour = atof(buf);
        if (v->price_per_hour > 0)
            break;
        printf("Must be positive.\n");
    }
    while (1)
    {
        getInput("Price per day: ", buf, sizeof(buf));
        v->price_per_day = atof(buf);
        if (v->price_per_day > 0)
            break;
        printf("Must be positive.\n");
    }
    while (1)
    {
        getInput("Mileage (km): ", buf, sizeof(buf));
        v->mileage = atoi(buf);
        if (v->mileage >= 0)
            break;
        printf("Must be non-negative.\n");
    }
    v->next = *head;
    *head = v;
    saveVehicles(*head);
    printf("Vehicle added! ID: %d\n", v->id);
}

// Update an existing vehicle (Admin)
void updateVehicle(Vehicle *head)
{
    char buf[30];
    int id;
    getInput("Enter vehicle ID to update: ", buf, sizeof(buf));
    id = atoi(buf);
    Vehicle *v = findVehicleById(head, id);
    if (!v)
    {
        printf("Vehicle not found!\n");
        return;
    }
    printf("--- Update Vehicle (leave blank to keep old) ---\n");

    getInput("New model: ", buf, sizeof(buf));
    if (strlen(buf))
        strcpy(v->model, buf);

    getInput("New type: ", buf, sizeof(buf));
    if (strlen(buf))
        strcpy(v->type, buf);

    getInput("Available (1/0): ", buf, sizeof(buf));
    if (strlen(buf) && (buf[0] == '1' || buf[0] == '0'))
        v->available = atoi(buf);

    getInput("New price per hour: ", buf, sizeof(buf));
    if (strlen(buf))
        v->price_per_hour = atof(buf);

    getInput("New price per day: ", buf, sizeof(buf));
    if (strlen(buf))
        v->price_per_day = atof(buf);

    getInput("New mileage: ", buf, sizeof(buf));
    if (strlen(buf))
        v->mileage = atoi(buf);

    printf("Vehicle updated.\n");
    saveVehicles(head);
}

// Soft-delete a vehicle (Admin)
void softDeleteVehicle(Vehicle *head)
{
    char buf[20];
    int id;
    getInput("Enter vehicle ID to delete: ", buf, sizeof(buf));
    id = atoi(buf);
    Vehicle *v = findVehicleById(head, id);
    if (!v)
    {
        printf("Vehicle not found!\n");
        return;
    }
    v->active = 0;
    printf("Vehicle soft-deleted.\n");
    saveVehicles(head);
}

// Show all active & available vehicles (for customers)
void displayAvailableVehicles(Vehicle *head)
{
    printf("\n--- Available Vehicles ---\n");
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->active && v->available)
            printf("ID: %d | %s | %s | Per Hour: %.2f | Per Day: %.2f | Mileage: %d km | Rented: %d times\n",
                   v->id, v->model, v->type, v->price_per_hour, v->price_per_day, v->mileage, v->popularity);
    }
}

// Show all vehicles (admin)
void displayAllVehicles(Vehicle *head)
{
    printf("\n--- All Vehicles ---\n");
    for (Vehicle *v = head; v; v = v->next)
    {
        printf("ID: %d | %s | %s | Available: %d | Per Hour: %.2f | Per Day: %.2f | Mileage: %d | Rented: %d | Status: %s\n",
               v->id, v->model, v->type, v->available, v->price_per_hour, v->price_per_day,
               v->mileage, v->popularity, v->active ? "Active" : "Inactive");
    }
}

// Admin vehicle menu
void adminVehicleMenu(Vehicle **head)
{
    char input[10];
    int option;
    while (1)
    {
        printf("\n--- Admin Vehicle Menu ---\n");
        printf("1. Add Vehicle\n2. Update Vehicle\n3. Soft Delete Vehicle\n4. List All Vehicles\n5. Export CSV\n6. Import CSV\n7. Back\nChoose: ");
        getInput("", input, sizeof(input));
        option = atoi(input);
        if (option == 1)
            addVehicle(head);
        else if (option == 2)
            updateVehicle(*head);
        else if (option == 3)
            softDeleteVehicle(*head);
        else if (option == 4)
            displayAllVehicles(*head);
        else if (option == 5)
        {
            saveVehicles(*head);
            printf("Exported vehicles.csv\n");
        }
        else if (option == 6)
        {
            loadVehicles(head);
            printf("Imported vehicles.csv\n");
        }
        else if (option == 7)
            break;
        else
            printf("Invalid!\n");
    }
}

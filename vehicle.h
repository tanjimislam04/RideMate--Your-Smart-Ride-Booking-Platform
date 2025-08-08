#ifndef VEHICLE_H
#define VEHICLE_H

typedef struct Vehicle
{
    int id;
    char model[50];
    char type[30];        // e.g., "Car", "Microbus", "Bike"
    int available;        // 1=available, 0=not available
    float price_per_hour; // Hourly rent rate
    float price_per_day;  // Daily rent rate
    int popularity;       // Number of times rented
    int mileage;          // in kilometers (for sorting/filter)
    int active;           // For soft delete (1=active, 0=inactive)
    struct Vehicle *next;
} Vehicle;

// Vehicle file I/O
void loadVehicles(Vehicle **head);
void saveVehicles(Vehicle *head);

// CRUD operations
void addVehicle(Vehicle **head);
void updateVehicle(Vehicle *head);
void softDeleteVehicle(Vehicle *head);
Vehicle *findVehicleById(Vehicle *head, int id);

void displayAvailableVehicles(Vehicle *head);
void displayAllVehicles(Vehicle *head);

// Admin menu
void adminVehicleMenu(Vehicle **head);

#endif

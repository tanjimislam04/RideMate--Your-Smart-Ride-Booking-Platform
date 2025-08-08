#ifndef VEHICLE_H
#define VEHICLE_H

#include <stdio.h>

/* ===== Vehicle & Route Types ===== */

typedef enum
{
    VEH_CAR = 0,
    VEH_MOTORCYCLE = 1,
    VEH_TRUCK = 2,
    VEH_VAN = 3
} VehicleType;

typedef struct Vehicle
{
    int id;
    char make[50];
    char model[50];
    int year;
    VehicleType type;
    float ratePerDay;
    float ratePerHour;
    int active;    /* 1 = active (not soft-deleted) */
    int available; /* 1 = available to rent */
    struct Vehicle *next;
} Vehicle;

typedef struct Route
{
    int id;
    char name[64];
    char from[64];
    char to[64];
    float baseFare;
    int etaMin; /* minutes */
    int active; /* 1 = active (not soft-deleted) */
    struct Route *next;
} Route;

/* Global route list head (used by rental.c) */
extern Route *routeHead;

/* ===== Vehicle CSV I/O ===== */
void loadVehicles(Vehicle **headRef);
void saveVehicles(Vehicle *head);

/* ===== Route CSV I/O ===== */
void loadRoutes(Route **headRef);
void saveRoutes(Route *head);

/* ===== Admin Menu ===== */
void adminVehicleMenu(Vehicle **headRef);

/* ===== Helpers used by other modules ===== */
void displayAvailableVehicles(Vehicle *head);
Vehicle *findVehicleById(Vehicle *head, int id);

void displayAllRoutes(Route *head);
Route *findRouteById(Route *head, int id);

#endif /* VEHICLE_H */

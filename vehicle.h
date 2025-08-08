#ifndef VEHICLE_H
#define VEHICLE_H

#include <stdio.h>

/* ===== Vehicle & Route Types ===== */

typedef enum
{
    VTYPE_CAR = 1,
    VTYPE_MOTORCYCLE = 2,
    VTYPE_TRUCK = 3,
    VTYPE_VAN = 4
} VehicleType;

typedef struct Vehicle
{
    int id;
    char make[50];
    char model[50];
    int year;
    VehicleType type;
    float ratePerHour;
    float ratePerDay;
    int available; /* 1 = available, 0 = not available */
    int active;    /* 1 = active, 0 = soft-deleted */
    char licensePlate[20];

    struct Vehicle *next;
} Vehicle;

typedef struct Route
{
    int id;
    char start[50];
    char end[50];
    float baseFare;
    int etaMin; /* estimated minutes */

    struct Route *next;
} Route;

/* Global route list head (used by rental.c) */
extern Route *routeHead;

/* ===== Vehicle CSV I/O ===== */
void loadVehicles(Vehicle **head);
void saveVehicles(Vehicle *head);

/* ===== Route CSV I/O ===== */
void loadRoutes(Route **head);
void saveRoutes(Route *head);

/* ===== Admin Menus ===== */
void adminVehicleMenu(Vehicle **head);
void adminRoutesMenu(Route **rhead);

/* ===== Helpers used by other modules ===== */
void displayAvailableVehicles(Vehicle *head);
Vehicle *findVehicleById(Vehicle *head, int id);

void displayAllRoutes(Route *rhead);
Route *findRouteById(Route *rhead, int routeId);

/* ===== Pretty strings ===== */
const char *vehicleTypeStr(VehicleType t);

#endif /* VEHICLE_H */

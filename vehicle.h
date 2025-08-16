#ifndef VEHICLE_H
#define VEHICLE_H

#include "utils.h"

typedef enum
{
    VTYPE_CAR,
    VTYPE_MOTORCYCLE,
    VTYPE_TRUCK,
    VTYPE_VAN
} VehicleType;

typedef struct RouteNode
{
    int id;
    char name[MAX_STRING];
    char from[MAX_STRING];
    char to[MAX_STRING];
    float baseFare;
    int etaMin;
    int active;
    struct RouteNode *next;
} Route;

typedef struct VehicleNode
{
    int id;
    char make[MAX_STRING];
    char model[MAX_STRING];
    int year;
    VehicleType type;
    float ratePerHour;
    float ratePerDay;
    int available;
    int active;
    struct VehicleNode *next;
} Vehicle;

extern Route *routeHead;

void loadVehicles(Vehicle **head);
void saveVehicles(Vehicle *head);
void freeVehicleList(Vehicle **head);
void loadRoutes(Route **head);
void saveRoutes(Route *head);
void freeRouteList(Route **head);
void adminVehicleMenu(Vehicle **head);
void displayVehicle(const Vehicle *v);
void listAllVehicles(Vehicle *head);
void displayAvailableVehicles(Vehicle *head);
Vehicle *findVehicleById(Vehicle *head, int id);
const char *vehicleTypeStr(VehicleType t);
void displayAllRoutes(Route *head);
Route *findRouteById(Route *head, int id);

#endif // VEHICLE_H
#ifndef VEHICLE_H
#define VEHICLE_H

#include "utils.h"

// Forward declaration for Rental type
typedef struct RentalNode Rental;

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
    int ratingCount;     // Number of ratings received
    float averageRating; // Average rating (1.0 - 5.0)
    struct VehicleNode *next;
} Vehicle;
void loadVehicles(Vehicle **head);
void saveVehicles(Vehicle *head);
void freeVehicleList(Vehicle **head);
void loadRoutes(Route **head);
void saveRoutes(Route *head);
void freeRouteList(Route **head);
void adminVehicleMenu(Vehicle **head, Rental *rentalHead);
void displayVehicle(const Vehicle *v);
void listAllVehicles(Vehicle *head);
void displayAvailableVehicles(Vehicle *head);
void displayVehicleAvailabilitySchedule(Vehicle *head, Rental *rentalHead);
Vehicle *findVehicleById(Vehicle *head, int id);
const char *vehicleTypeStr(VehicleType t);
void displayAllRoutes(Route *head);
Route *findRouteById(Route *head, int id);
void addVehicleRating(Vehicle *v, int rating);
void updateVehicleRating(Vehicle *head, int vehicleId, int newRating);
void displayTopRatedVehicles(Vehicle *head, int count);

#endif // VEHICLE_H
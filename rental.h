#ifndef RENTAL_H
#define RENTAL_H

#include "utils.h"

// Forward Declarations
typedef struct VehicleNode Vehicle;
typedef struct CustomerNode Customer;
typedef struct RouteNode Route;

typedef enum
{
    RENT_ACTIVE = 1,
    RENT_COMPLETED,
    RENT_CANCELLED
} RentalStatus;
typedef enum
{
    RENT_HOURLY = 1,
    RENT_DAILY,
    RENT_ROUTE
} RentalType;

typedef struct RentalNode
{
    int id;
    int customerId;
    int vehicleId;
    int routeId;
    RentalType type;
    RentalStatus status;
    char startTime[20];
    char endTime[20];
    float totalCost;
    struct RentalNode *next;
} Rental;

void loadRentals(Rental **head);
void saveRentals(Rental *head);
void freeRentalList(Rental **head);
void createRentalByCustomer(Rental **rentalHead, Vehicle *vehicleHead, Customer *current);
void displayRental(const Rental *r);
void listAllRentals(Rental *head);
void displayRentalsByCustomer(Rental *head, int customerId);
void completeRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead);
void cancelRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead);
const char *rentalTypeStr(RentalType t);
const char *rentalStatusStr(RentalStatus s);

#endif // RENTAL_H
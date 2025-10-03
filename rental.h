#ifndef RENTAL_H
#define RENTAL_H

#include "utils.h"
#include "promo.h"
#include "driver.h"
#include "invoice.h"

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
    int driverId; // Assigned driver ID, 0 if no driver assigned
    RentalType type;
    RentalStatus status;
    char startTime[20];
    char endTime[20];
    float totalCost;
    int vehicleRating; // Rating for this specific rental (1-5)
    int driverRating;  // Rating for this specific rental (1-5)
    char comment[51];  // Optional comment (max 50 chars + null terminator)
    struct RentalNode *next;
} Rental;

// Core rental management functions
void loadRentals(Rental **head);
void saveRentals(Rental *head);
void freeRentalList(Rental **head);
Rental *findRentalById(Rental *head, int rentalId);

// Conflict detection and validation functions
int isVehicleAvailableForTime(Rental *head, int vehicleId, time_t startTime, time_t endTime);
int validateRentalTimeRange(time_t startTime, time_t endTime, int rentalType);

// Rental lifecycle functions
int completeRental(Rental *r, Vehicle *vehicleHead, Driver *driverHead);
int cancelRental(Rental *r, Vehicle *vehicleHead, Driver *driverHead);
void completeRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead, Driver *driverHead);
void cancelRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead, Driver *driverHead);

// Display functions
void displayRental(const Rental *r);
void displayAllRentals(Rental *head);
void displayRentalsByCustomer(Rental *head, int customerId);
void listAllRentals(Rental *head);

// Customer booking function
void createRentalByCustomer(Rental **rentalHead, Vehicle *vehicleHead, Customer *current, Promo *promoHead, Driver *driverHead, Invoice **invoiceHead);

// Review display function
void displayVehicleReviews(Rental *rentalHead, int vehicleId);

#endif // RENTAL_H
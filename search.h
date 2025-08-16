#ifndef SEARCH_H
#define SEARCH_H

#include "vehicle.h"
#include "rental.h"

typedef enum
{
    SORT_PRICE_DAY,
    SORT_PRICE_HOUR,
    SORT_YEAR
} VehicleSortField;
typedef enum
{
    SORT_ASC,
    SORT_DESC
} SortOrder;

void searchVehiclesByText(const Vehicle *head, const char *query);
void filterVehiclesByType(const Vehicle *head, const char *type);
void filterVehiclesByPrice(const Vehicle *head, float maxPrice);
void sortVehicles(Vehicle **head, VehicleSortField field, SortOrder order);
void searchRentalsByCustomerId(const Rental *head, int customerId);
void adminSearchMenu(Vehicle *vehicleHead, Rental *rentalHead);

#endif // SEARCH_H

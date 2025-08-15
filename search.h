// File: search.h
// Just the function prototypes for our new search/filter features.

#ifndef SEARCH_H
#define SEARCH_H

#include "vehicle.h" // We need to know what a 'Vehicle' is
#include "rental.h"  // And what a 'Rental' is

// --- Vehicle Search & Filter ---
void searchVehiclesByText(Vehicle* head, const char* query);
void filterVehiclesByType(Vehicle* head, VehicleType type);
void filterVehiclesByPrice(Vehicle* head, float maxPrice);

// --- Rental Search ---
void searchRentalsByCustomerId(Rental* head, int customerId);

// --- The Admin Sub-Menu for this module ---
void adminSearchMenu(Vehicle* vehicleHead, Rental* rentalHead);


#endif // SEARCH_H
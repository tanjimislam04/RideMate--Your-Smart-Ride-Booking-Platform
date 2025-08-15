// File: dashboard.h
// Description: Declares the function for displaying the admin dashboard.

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "vehicle.h"
#include "customer.h"
#include "rental.h"

// The main function that calculates and displays all statistics
void showAdminDashboard(Vehicle* vehicleHead, Customer* customerHead, Rental* rentalHead);

#endif // DASHBOARD_H
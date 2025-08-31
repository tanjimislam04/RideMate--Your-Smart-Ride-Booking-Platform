// File: alert.h
// Description: Declares the functions for the terminal-based alert system.

#ifndef ALERT_H
#define ALERT_H

// Forward declarations to keep this header independent
struct RentalNode;
struct VehicleNode;

// Displays system alerts (e.g., overdue rentals) at the top of the admin menu.
void displayAdminAlerts(struct RentalNode *rentalHead, struct VehicleNode *vehicleHead);

#endif // ALERT_H

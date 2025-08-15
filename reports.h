// File: report.h
// Description: Declares functions for generating detailed business reports.

#ifndef REPORT_H
#define REPORT_H

#include "vehicle.h"
#include "customer.h"
#include "rental.h"

// The main menu function for the reports module, called from the admin panel.
void adminReportsMenu(Vehicle* vehicleHead, Customer* customerHead, Rental* rentalHead);
#endif // REPORT_H

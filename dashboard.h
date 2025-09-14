#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "vehicle.h"
#include "customer.h"
#include "rental.h"

void showAdminDashboard(Vehicle *vehicleHead, Customer *customerHead, Rental *rentalHead);
void showBookingCalendar(Vehicle *vehicleHead, Rental *rentalHead);

#endif // DASHBOARD_H
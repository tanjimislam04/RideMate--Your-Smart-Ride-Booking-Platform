#include "alert.h"
#include "utils.h"
#include "rental.h"
#include "vehicle.h"
#include <stdio.h>
#include <time.h>

void displayAdminAlerts(Rental *rentalHead, Vehicle *vehicleHead)
{
    time_t now = time(NULL);
    int overdue_count = 0;
    int maintenance_count = 0;
    int upcoming_conflicts = 0;

    // Check for overdue rentals
    for (Rental *r = rentalHead; r; r = r->next)
    {
        if (r->status == RENT_ACTIVE)
        {
            time_t endTime;
            if (stringToTime(r->endTime, &endTime))
            {
                if (now > endTime)
                {
                    overdue_count++;
                }
            }
        }
    }

    // Check for vehicles in maintenance
    for (Vehicle *v = vehicleHead; v; v = v->next)
    {
        if (v->active == 0)
        {
            maintenance_count++;
        }
    }

    // Check for potential booking conflicts in next 24 hours
    time_t tomorrow = now + (24 * 60 * 60);
    for (Vehicle *v = vehicleHead; v; v = v->next)
    {
        if (v->active && v->available)
        {
            // Count active rentals for this vehicle that end within 24 hours
            int vehicleRentals = 0;
            for (Rental *r = rentalHead; r; r = r->next)
            {
                if (r->vehicleId == v->id && r->status == RENT_ACTIVE)
                {
                    time_t endTime;
                    if (stringToTime(r->endTime, &endTime))
                    {
                        if (endTime <= tomorrow && endTime > now)
                        {
                            vehicleRentals++;
                        }
                    }
                }
            }
            if (vehicleRentals > 1)
            {
                upcoming_conflicts++;
            }
        }
    }

    if (overdue_count > 0 || maintenance_count > 0 || upcoming_conflicts > 0)
    {
        printf("\n+-------------------------------------------+\n");
        printf("|              SYSTEM ALERTS                  |\n");
        printf("+--------------------------------------------+\n");
        if (overdue_count > 0)
        {
            printf("| %-41s |\n", "ATTENTION: You have overdue rentals!");
            printf("| Count: %-33d |\n", overdue_count);
        }
        if (maintenance_count > 0)
        {
            printf("| %-41s |\n", "INFO: Vehicles currently in maintenance");
            printf("| Count: %-33d |\n", maintenance_count);
        }
        if (upcoming_conflicts > 0)
        {
            printf("| %-41s |\n", "WARNING: Potential booking conflicts detected");
            printf("| Count: %-33d |\n", upcoming_conflicts);
        }
        printf("+-------------------------------------------+\n");
    }
}

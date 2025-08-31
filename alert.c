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

    for (Vehicle *v = vehicleHead; v; v = v->next)
    {
        if (v->active == 0)
        {
            maintenance_count++;
        }
    }

    if (overdue_count > 0 || maintenance_count > 0)
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
        printf("+-------------------------------------------+\n");
    }
}

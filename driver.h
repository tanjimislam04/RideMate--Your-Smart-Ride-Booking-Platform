#ifndef DRIVER_H
#define DRIVER_H

#include <time.h>

// --- Driver Status Enum ---
typedef enum
{
    DRIVER_AVAILABLE = 0,
    DRIVER_BUSY = 1,
    DRIVER_OFFLINE = 2
} DriverStatus;

// --- Driver Struct (Node for Linked List) ---
typedef struct DriverNode
{
    int id;                  // Unique driver ID
    char name[50];           // Driver's full name
    char phone[15];          // Contact number
    char licenseNumber[20];  // Driving license number
    char vehicleType[20];    // Type of vehicle they can drive
    float rating;            // Average rating (0.0 - 5.0)
    int totalTrips;          // Total number of completed trips
    int totalEarnings;       // Total earnings in dollars
    DriverStatus status;     // Current availability status
    time_t lastActive;       // Last active timestamp
    struct DriverNode *next; // Pointer to next driver
} Driver;

// --- Function Prototypes ---

// CSV I/O Functions
void loadDrivers(Driver **head);
void saveDrivers(Driver *head);

// Driver Management Functions
Driver *findDriverById(Driver *head, int driverId);
Driver *findAvailableDriver(Driver *head, const char *vehicleType);
void addDriver(Driver **head);
void updateDriverProfile(Driver *driver);
void displayDriverProfile(const Driver *driver);
void listAllDrivers(Driver *head);
void listAvailableDrivers(Driver *head);

// Driver Assignment Functions
Driver *assignDriverToRental(Driver *head, const char *vehicleType);
void updateDriverStatus(Driver *driver, DriverStatus status);
void updateDriverRating(Driver *driver, float newRating);
void completeDriverTrip(Driver *driver, float tripEarnings);

// Driver Statistics
void showDriverStatistics(Driver *head);
void showTopDrivers(Driver *head, int count);

// Memory Management
void freeDriverList(Driver **head);

#endif // DRIVER_H

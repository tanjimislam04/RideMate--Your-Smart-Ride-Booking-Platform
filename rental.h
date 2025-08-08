#ifndef RENTAL_H
#define RENTAL_H

/* Forward declarations to avoid circular includes. */
typedef struct Vehicle Vehicle;
typedef struct Customer Customer;

/* Rental lifecycle status */
typedef enum
{
    RENT_ACTIVE = 0,
    RENT_COMPLETED = 1,
    RENT_CANCELLED = 2
} RentalStatus;

/* Rental types */
typedef enum
{
    RENT_HOURLY = 1,
    RENT_DAILY = 2,
    RENT_ROUTE = 3
} RentalType;

/* Rental node (linked list) */
typedef struct Rental
{
    int id;
    int customerId;
    int vehicleId;
    int routeId; // >0 only for RENT_ROUTE, otherwise 0
    RentalType type;
    char startTime[20]; // "YYYY-MM-DD HH:MM"
    char endTime[20];   // estimated or actual on completion
    float totalCost;
    RentalStatus status;
    struct Rental *next;
} Rental;

/* CSV I/O */
void loadRentals(Rental **head);
void saveRentals(Rental *head);

/* Creation & display */
void createRentalByCustomer(Rental **rentalHead, Vehicle *vehicleHead, Customer *current);
void displayAllRentals(Rental *head);
void displayRentalsByCustomer(Rental *head, int customerId);

/* Find & lifecycle */
Rental *findRentalById(Rental *head, int rentalId);
int completeRental(Rental *r, Vehicle *vehicleHead);
int cancelRental(Rental *r, Vehicle *vehicleHead);

/* Convenience prompts for menus (admin/customer) */
void completeRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead);
void cancelRentalPrompt(Rental *rentalHead, Vehicle *vehicleHead);

#endif /* RENTAL_H */

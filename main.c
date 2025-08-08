// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "vehicle.h"
#include "customer.h"
#include "rental.h"

// Global linked-list heads
Vehicle *vehicleHead = NULL;
Customer *customerHead = NULL;
Rental *rentalHead = NULL;

// Menus
static void displayMainMenu(void);
static void adminMenu(void);
static void customerMenu(Customer *current);

int main(void)
{
    // Load all system data
    loadVehicles(&vehicleHead);
    loadCustomers(&customerHead);
    loadRentals(&rentalHead);

    int running = 1;
    char input[16];

    while (running)
    {
        displayMainMenu();
        getInput("Enter your choice: ", input, sizeof(input));
        if (!isValidNumber(input))
        {
            printf("Invalid input! Please enter a number.\n");
            continue;
        }
        int choice = atoi(input);

        switch (choice)
        {
        case 1:
        { // Customer Login
            char username[50], password[50];
            getInput("Enter username: ", username, sizeof(username));
            getInput("Enter password: ", password, sizeof(password));

            Customer *loggedIn = authenticateCustomer(customerHead, username, password);
            if (loggedIn && loggedIn->active)
            {
                printf("Login successful! Welcome, %s\n", loggedIn->name);
                customerMenu(loggedIn);
            }
            else
            {
                printf("Login failed! Invalid username or password.\n");
            }
            break;
        }
        case 2: // New Customer Registration
            registerCustomer(&customerHead);
            break;

        case 3:
        { // Admin Login
            char adminUsername[50], adminPassword[50];
            getInput("Enter admin username: ", adminUsername, sizeof(adminUsername));
            getInput("Enter admin password: ", adminPassword, sizeof(adminPassword));
            if (authenticateAdmin(adminUsername, adminPassword))
            {
                printf("Admin login successful!\n");
                adminMenu();
            }
            else
            {
                printf("Admin login failed! Invalid username or password.\n");
            }
            break;
        }

        case 4: // Exit
            printf("Saving data and exiting...\n");
            saveVehicles(vehicleHead);
            saveCustomers(customerHead);
            saveRentals(rentalHead);
            running = 0;
            break;

        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}

static void displayMainMenu(void)
{
    printf("\n=== RideMate : Your Smart Ride Booking Platform ===\n");
    printf("1. Customer Login\n");
    printf("2. New Customer Registration\n");
    printf("3. Admin Login\n");
    printf("4. Exit\n");
}

static void adminMenu(void)
{
    char input[16];
    int option;

    while (1)
    {
        printf("\n=== Admin Menu ===\n");
        printf("1. Manage Vehicles\n");
        printf("2. Manage Customers\n");
        printf("3. View Rentals\n");
        printf("4. Back to Main Menu\n");
        getInput("Enter your choice: ", input, sizeof(input));
        if (!isValidNumber(input))
        {
            printf("Invalid input! Please enter a number.\n");
            continue;
        }
        option = atoi(input);

        switch (option)
        {
        case 1:
            adminVehicleMenu(&vehicleHead); // from vehicle.c
            break;
        case 2:
            adminCustomerMenu(&customerHead); // from customer.c
            break;
        case 3:
            displayAllRentals(rentalHead); // from rental.c (can be stubbed for now)
            break;
        case 4:
            // Persist data whenever you return
            saveVehicles(vehicleHead);
            saveCustomers(customerHead);
            saveRentals(rentalHead);
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

static void customerMenu(Customer *current)
{
    char input[16];
    int option;

    while (1)
    {
        printf("\n=== Customer Menu ===\n");
        printf("1. View Profile\n");
        printf("2. Update Profile\n");
        printf("3. View Available Vehicles\n");
        printf("4. Rent a Vehicle (hour/day/route)\n");
        printf("5. View My Rentals\n");
        printf("6. Logout\n");
        getInput("Enter your choice: ", input, sizeof(input));
        if (!isValidNumber(input))
        {
            printf("Invalid input! Please enter a number.\n");
            continue;
        }
        option = atoi(input);

        switch (option)
        {
        case 1:
            displayCustomer(current); // from customer.c
            break;

        case 2:
            updateCustomerProfile(current); // from customer.c
            saveCustomers(customerHead);
            break;

        case 3:
            displayAvailableVehicles(vehicleHead); // from vehicle.c
            break;

        case 4:
            // Implement in rental.c (hourly/daily/route booking flow)
            createRentalByCustomer(&rentalHead, vehicleHead, current);
            // Optional: save immediately after booking
            saveRentals(rentalHead);
            saveVehicles(vehicleHead);
            break;

        case 5:
            displayRentalsByCustomer(rentalHead, current->id); // from rental.c
            break;

        case 6:
            // Persist any profile changes on logout
            saveCustomers(customerHead);
            return;

        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

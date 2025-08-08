#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "vehicle.h"
#include "customer.h"
#include "rental.h"

// Linked-list heads (globals)
Vehicle *vehicleHead = NULL;
Customer *customerHead = NULL;
Rental *rentalHead = NULL;

// Menus
static void displayMainMenu(void);
static void adminMenu(void);
static void adminRentalsMenu(void);
static void customerMenu(Customer *current);

int main(void)
{
    // Load persisted data
    loadVehicles(&vehicleHead);   // vehicles.csv (+ routes loaded inside vehicle module if you do it there)
    loadCustomers(&customerHead); // customers.csv
    loadRentals(&rentalHead);     // rentals.csv
    loadRoutes(&routeHead);

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
        {
            // Customer Login
            char username[50], password[50];
            getInput("Enter username: ", username, sizeof(username));
            getInput("Enter password: ", password, sizeof(password));
            Customer *loggedIn = authenticateCustomer(customerHead, username, password);
            if (loggedIn)
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
        case 2:
            // Registration
            registerCustomer(&customerHead);
            break;
        case 3:
        {
            // Admin Login
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
        case 4:
            // Exit + save
            printf("Exiting the program...\n");
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
        printf("1. Manage Vehicles (incl. Routes)\n");
        printf("2. Manage Customers\n");
        printf("3. Manage/View Rentals\n");
        printf("4. Save All\n");
        printf("5. Back to Main Menu\n");

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
            // vehicle module handles vehicle & route management
            adminVehicleMenu(&vehicleHead);
            break;
        case 2:
            adminCustomerMenu(&customerHead);
            break;
        case 3:
            adminRentalsMenu();
            break;
        case 4:
            saveVehicles(vehicleHead);
            saveCustomers(customerHead);
            saveRentals(rentalHead);
            printf("All data saved.\n");
            break;
        case 5:
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

static void adminRentalsMenu(void)
{
    char input[16];
    int opt;

    while (1)
    {
        printf("\n=== Admin Rentals ===\n");
        printf("1. View All Rentals\n");
        printf("2. Complete a Rental\n");
        printf("3. Cancel a Rental\n");
        printf("4. Back\n");

        getInput("Choose: ", input, sizeof(input));
        if (!isValidNumber(input))
        {
            printf("Invalid input! Please enter a number.\n");
            continue;
        }
        opt = atoi(input);

        switch (opt)
        {
        case 1:
            displayAllRentals(rentalHead);
            break;
        case 2:
            completeRentalPrompt(rentalHead, vehicleHead);
            break;
        case 3:
            cancelRentalPrompt(rentalHead, vehicleHead);
            break;
        case 4:
            return;
        default:
            printf("Invalid.\n");
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
        printf("4. Rent a Vehicle (Hourly/Daily/Route)\n");
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
            displayCustomer(current);
            break;
        case 2:
            updateCustomerProfile(current);
            // persist profile changes
            saveCustomers(customerHead);
            break;
        case 3:
            displayAvailableVehicles(vehicleHead);
            break;
        case 4:
            createRentalByCustomer(&rentalHead, vehicleHead, current);
            break;
        case 5:
            displayRentalsByCustomer(rentalHead, current->id);
            break;
        case 6:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

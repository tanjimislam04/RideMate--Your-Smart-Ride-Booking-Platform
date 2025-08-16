// File: main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "vehicle.h"
#include "customer.h"
#include "rental.h"
#include "search.h"
#include "dashboard.h"
#include "reports.h"

// Linked-list heads
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
    loadVehicles(&vehicleHead);
    loadCustomers(&customerHead);
    loadRentals(&rentalHead);
    loadRoutes(&routeHead);

    int running = 1;
    while (running)
    {
        displayMainMenu();
        int choice = getIntegerInput("Enter your choice: ", 1, 4);

        switch (choice)
        {
        case 1:
        {
            char username[MAX_STRING], password[MAX_STRING];
            getStringInput("Enter Customer Username: ", username, MAX_STRING);
            getStringInput("Enter Customer Password: ", password, MAX_STRING);
            Customer *loggedIn = authenticateCustomer(customerHead, username, password);
            if (loggedIn)
            {
                printf("Login successful! Welcome, %s\n", loggedIn->name);
                pressEnterToContinue();
                customerMenu(loggedIn);
            }
            else
            {
                printf("Login failed! Invalid username or password.\n");
                pressEnterToContinue();
            }
            break;
        }
        case 2:
            registerCustomer(&customerHead);
            pressEnterToContinue();
            break;
        case 3:
        {
            char adminUsername[MAX_STRING], adminPassword[MAX_STRING];
            getStringInput("Enter Admin Username: ", adminUsername, MAX_STRING);
            getStringInput("Enter Admin Password: ", adminPassword, MAX_STRING);
            if (authenticateAdmin(adminUsername, adminPassword))
            {
                printf("Admin login successful!\n");
                pressEnterToContinue();
                adminMenu();
            }
            else
            {
                printf("Admin login failed! Invalid username or password.\n");
                pressEnterToContinue();
            }
            break;
        }
        case 4:
            printf("\nSaving all data...\n");
            saveVehicles(vehicleHead);
            saveCustomers(customerHead);
            saveRentals(rentalHead);
            saveRoutes(routeHead);

            // Free all allocated memory
            freeVehicleList(&vehicleHead);
            freeCustomerList(&customerHead);
            freeRentalList(&rentalHead);
            freeRouteList(&routeHead);

            printf("Exiting RideMate. Goodbye!\n");
            running = 0;
            break;
        }
    }
    return 0;
}

static void displayMainMenu(void)
{
    clearScreen();
    printf("================================================\n");
    printf("     Welcome to RideMate Rental System\n");
    printf("================================================\n");
    printf("1. Customer Login\n");
    printf("2. New Customer Registration\n");
    printf("3. Admin Login\n");
    printf("4. Exit\n");
    printf("------------------------------------------------\n");
}

static void adminMenu(void)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Admin Panel ---\n");
        printf("1. Manage Vehicles & Routes\n");
        printf("2. Manage Customers\n");
        printf("3. Manage Rentals\n");
        printf("4. Search & Filter\n");
        printf("5. View Dashboard\n");
        printf("6. Generate Reports\n");
        printf("7. Logout\n");
        int choice = getIntegerInput("Enter your choice: ", 1, 7);

        switch (choice)
        {
        case 1:
            adminVehicleMenu(&vehicleHead);
            break;
        case 2:
            adminCustomerMenu(&customerHead);
            break;
        case 3:
            adminRentalsMenu();
            break;
        case 4:
            adminSearchMenu(vehicleHead, rentalHead);
            break;
        case 5:
            showAdminDashboard(vehicleHead, customerHead, rentalHead);
            pressEnterToContinue();
            break;
        case 6:
            adminReportsMenu(vehicleHead, customerHead, rentalHead);
            break;
        case 7:
            running = 0;
            break;
        }
    }
}

static void adminRentalsMenu(void)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Admin Rentals Management ---\n");
        printf("1. View All Rentals\n");
        printf("2. Complete a Rental\n");
        printf("3. Cancel a Rental\n");
        printf("4. Back\n");
        int choice = getIntegerInput("Choose: ", 1, 4);
        switch (choice)
        {
        case 1:
            listAllRentals(rentalHead);
            break;
        case 2:
            completeRentalPrompt(rentalHead, vehicleHead);
            break;
        case 3:
            cancelRentalPrompt(rentalHead, vehicleHead);
            break;
        case 4:
            running = 0;
            break;
        }
        if (running)
            pressEnterToContinue();
    }
}

static void customerMenu(Customer *current)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Customer Panel (Welcome, %s) ---\n", current->name);
        printf("1. View My Profile\n");
        printf("2. Update My Profile\n");
        printf("3. View Available Vehicles\n");
        printf("4. Rent a Vehicle\n");
        printf("5. View My Rentals\n");
        printf("6. Logout\n");
        int choice = getIntegerInput("Enter your choice: ", 1, 6);
        switch (choice)
        {
        case 1:
            displayCustomerProfile(current);
            break;
        case 2:
            updateCustomerProfile(current);
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
            running = 0;
            break;
        }
        if (running)
            pressEnterToContinue();
    }
    printf("Logging out...\n");
}
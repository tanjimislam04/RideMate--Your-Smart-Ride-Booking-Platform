#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "vehicle.h"
#include "customer.h"
#include "rental.h"

// Declare linked list heads (global pointers)
Vehicle *vehicleHead = NULL;
Customer *customerHead = NULL;
Rental *rentalHead = NULL;

// Function prototypes
void displayMainMenu();
void adminMenu();
void customerMenu(Customer *current);

int main()
{
    // Load all system data into linked lists
    loadVehicles(&vehicleHead);
    loadCustomers(&customerHead);
    loadRentals(&rentalHead);

    int running = 1;
    char input[10];

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
        case 4:
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

void displayMainMenu()
{
    printf("\n=== RideMate : Your Smart Ride Booking Platform ===\n");
    printf("1. Customer Login\n");
    printf("2. New Customer Registration\n");
    printf("3. Admin Login\n");
    printf("4. Exit\n");
}

void adminMenu()
{
    char input[10];
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
            adminVehicleMenu(&vehicleHead);
            break;
        case 2:
            adminCustomerMenu(&customerHead);
            break;
        case 3:
            displayAllRentals(rentalHead);
            break;
        case 4:
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

void customerMenu(Customer *current)
{
    char input[10];
    int option;
    while (1)
    {
        printf("\n=== Customer Menu ===\n");
        printf("1. View Profile\n");
        printf("2. Update Profile\n");
        printf("3. View Available Vehicles\n");
        printf("4. Rent a Vehicle\n");
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

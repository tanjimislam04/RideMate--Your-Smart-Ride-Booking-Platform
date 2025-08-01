#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "vehicle.h"
#include "customer.h"
#include "rental.h"

// Function prototypes for menu
void displayMainMenu();
void adminMenu();
void customerMenu(int customerIndex);

int main() {
    int running = 1;
    // Load all system data (must call these functions!)
    loadVehicles();
    loadCustomers();
    loadRentals();

    while (running) {
        displayMainMenu();
        int choice;
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear newline character from input buffer
        switch (choice) {
            case 1: {
                // Customer Login
                char username[50], password[50];
                printf("Enter username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0;
                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = 0;
                int idx = authenticateCustomer(username, password);
                if (idx >= 0) {
                    printf("Login successful! Welcome, %s\n", customers[idx].name);
                    customerMenu(idx);
                } else {
                    printf("Login failed! Invalid username or password.\n");
                }
                break;
            }
            case 2:
                // Customer Registration
                registerCustomer();
                break;
            case 3: {
                // Admin Login
                char adminUsername[50], adminPassword[50];
                printf("Enter admin username: ");
                fgets(adminUsername, sizeof(adminUsername), stdin);
                adminUsername[strcspn(adminUsername, "\n")] = 0;
                printf("Enter admin password: ");
                fgets(adminPassword, sizeof(adminPassword), stdin);
                adminPassword[strcspn(adminPassword, "\n")] = 0;
                if (authenticateAdmin(adminUsername, adminPassword)) {
                    printf("Admin login successful!\n");
                    adminMenu();
                } else {
                    printf("Admin login failed! Invalid username or password.\n");
                }
                break;
            }
            case 4:
                // Exit the program
                printf("Exiting the program...\n");
                saveVehicles();
                saveCustomers();
                saveRentals();
                running = 0;
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}

void displayMainMenu() {
    printf("\n=== RideMate : Your Smart Ride Booking Platform ===\n");
    printf("1. Customer Login\n");
    printf("2. New Customer Registration\n");
    printf("3. Admin Login\n");
    printf("4. Exit\n");
}

void adminMenu() {
    int option;
    while (1) {
        printf("\n=== Admin Menu ===\n");
        printf("1. Manage Vehicles\n");
        printf("2. Manage Customers\n");
        printf("3. View Rentals\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &option);
        getchar(); // Clear newline character

        switch (option) {
            case 1:
                adminVehicleMenu(); // Implement in vehicle.c
                break;
            case 2:
                adminCustomerMenu(); // Implement in customer.c
                break;
            case 3:
                displayAllRentals(); // Implement in rental.c
                break;
            case 4:
                return;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
}

void customerMenu(int customerIndex) {
    int option;
    while (1) {
        printf("\n=== Customer Menu ===\n");
        printf("1. View Profile\n");
        printf("2. Update Profile\n");
        printf("3. View Available Vehicles\n");
        printf("4. Rent a Vehicle\n");
        printf("5. View My Rentals\n");
        printf("6. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &option);
        getchar(); // Clear newline character

        switch(option) {
            case 1:
                displayCustomer(customers[customerIndex]);
                break;
            case 2:
                updateCustomerProfile(customerIndex); // Implement in customer.c
                break;
            case 3:
                displayAvailableVehicles(); // Implement in vehicle.c
                break;
            case 4:
                createRentalByCustomer(customers[customerIndex].id); // rental.c
                break;
            case 5:
                displayRentalsByCustomer(customers[customerIndex].id); // rental.c
                break;
            case 6:
                printf("Logging out...\n");
                return;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
}

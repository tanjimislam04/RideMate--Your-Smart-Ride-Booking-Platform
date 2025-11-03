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
#include "promo.h"
#include "driver.h"
#include "invoice.h"
#include "alert.h"
#include "backup.h"
#include "complaint.h"

Vehicle *vehicleHead = NULL;
Customer *customerHead = NULL;
Rental *rentalHead = NULL;
Promo *promoHead = NULL;
Driver *driverHead = NULL;
Invoice *invoiceHead = NULL;
Route *routeHead = NULL;
Complaint *complaintHead = NULL;

static void displayMainMenu(void);
static void adminMenu(void);
static void adminRentalsMenu(void);
static void adminDriverMenu(Driver **driverHead);
static void adminInvoiceMenu(Invoice **invoiceHead);
static void customerMenu(Customer *current);

int main(void)
{
    loadVehicles(&vehicleHead);
    loadCustomers(&customerHead);
    loadRentals(&rentalHead);
    loadRoutes(&routeHead);
    loadPromos(&promoHead);
    loadDrivers(&driverHead);
    loadInvoices(&invoiceHead);
    loadComplaints(&complaintHead);

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
            savePromos(promoHead);
            saveDrivers(driverHead);
            saveInvoices(invoiceHead);
            saveComplaints(complaintHead);

            freeVehicleList(&vehicleHead);
            freeCustomerList(&customerHead);
            freeRentalList(&rentalHead);
            freeRouteList(&routeHead);
            freePromoList(&promoHead);
            freeDriverList(&driverHead);
            freeInvoiceList(&invoiceHead);
            freeComplaintList(&complaintHead);

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

        displayAdminAlerts(rentalHead, vehicleHead);

        printf("\n--- Admin Panel ---\n");
        printf("1. Manage Vehicles & Routes\n");
        printf("2. Manage Customers\n");
        printf("3. Manage Rentals\n");
        printf("4. Search & Filter\n");
        printf("5. View Dashboard\n");
        printf("6. Generate Reports\n");
        printf("7. Manage Promo Codes\n");
        printf("8. View Booking Calendar\n");
        printf("9. Manage Drivers\n");
        printf("10. Manage Invoices\n");
        printf("11. Manage Complaints\n");
        printf("12. Data Management (Backup/Restore)\n");
        printf("13. Logout\n");
        int choice = getIntegerInput("Enter your choice: ", 1, 13);

        switch (choice)
        {
        case 1:
            adminVehicleMenu(&vehicleHead, rentalHead);
            break;
        case 2:
            adminCustomerMenu(&customerHead);
            saveCustomers(customerHead);
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
            adminPromoMenu(&promoHead);
            savePromos(promoHead);
            break;
        case 8:
            showBookingCalendar(vehicleHead, rentalHead);
            pressEnterToContinue();
            break;
        case 9:
            adminDriverMenu(&driverHead);
            saveDrivers(driverHead);
            break;
        case 10:
            adminInvoiceMenu(&invoiceHead);
            saveInvoices(invoiceHead);
            break;
        case 11:
            adminComplaintMenu(&complaintHead, rentalHead);
            break;
        case 12:
            adminBackupMenu();
            break;
        case 13:
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
            completeRentalPrompt(rentalHead, vehicleHead, driverHead);
            saveRentals(rentalHead);
            saveVehicles(vehicleHead);
            saveDrivers(driverHead);
            break;
        case 3:
            cancelRentalPrompt(rentalHead, vehicleHead, driverHead);
            saveRentals(rentalHead);
            saveVehicles(vehicleHead);
            saveDrivers(driverHead);
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
        printf("4. View Top Rated Vehicles\n");
        printf("5. Rent a Vehicle\n");
        printf("6. View My Rentals\n");
        printf("7. File a Complaint\n");
        printf("8. View My Complaints\n");
        printf("9. Logout\n");
        int choice = getIntegerInput("Enter your choice: ", 1, 9);
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
        {
            int count = getIntegerInput("Show top how many vehicles? (1-10): ", 1, 10);
            displayTopRatedVehicles(vehicleHead, count);
            break;
        }
        case 5:
            createRentalByCustomer(&rentalHead, vehicleHead, current, promoHead, driverHead, &invoiceHead);
            saveRentals(rentalHead);
            saveVehicles(vehicleHead);
            saveDrivers(driverHead);
            saveInvoices(invoiceHead);
            break;
        case 6:
            displayRentalsByCustomer(rentalHead, current->id);
            break;
        case 7:
        {
            // Show customer's rentals first
            displayRentalsByCustomer(rentalHead, current->id);
            if (rentalHead)
            {
                int rentalId = getIntegerInput("\nEnter Rental ID to file complaint for: ", 5001, 9999);
                Rental *rental = findRentalById(rentalHead, rentalId);
                if (rental && rental->customerId == current->id)
                {
                    fileComplaint(&complaintHead, rentalId, current->id);
                }
                else
                {
                    printf("Invalid rental ID or rental doesn't belong to you.\n");
                }
            }
            else
            {
                printf("No rentals found.\n");
            }
            break;
        }
        case 8:
            viewCustomerComplaints(complaintHead, current->id);
            break;
        case 9:
            running = 0;
            break;
        }
        if (running)
            pressEnterToContinue();
    }
    printf("Logging out...\n");
}

static void adminDriverMenu(Driver **driverHead)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Admin Driver Management ---\n");
        printf("1. Add New Driver\n");
        printf("2. View All Drivers\n");
        printf("3. View Available Drivers\n");
        printf("4. Update Driver Profile\n");
        printf("5. Driver Statistics\n");
        printf("6. Top Drivers\n");
        printf("7. Back\n");
        int choice = getIntegerInput("Choose: ", 1, 7);

        switch (choice)
        {
        case 1:
            addDriver(driverHead);
            break;
        case 2:
            listAllDrivers(*driverHead);
            break;
        case 3:
            listAvailableDrivers(*driverHead);
            break;
        case 4:
        {
            if (!*driverHead)
            {
                printf("No drivers found.\n");
                break;
            }
            listAllDrivers(*driverHead);
            int driverId = getIntegerInput("\nEnter Driver ID to update: ", 0, 9999);
            Driver *driver = findDriverById(*driverHead, driverId);
            if (driver)
            {
                updateDriverProfile(driver);
                saveDrivers(*driverHead);
            }
            else
            {
                printf("Driver not found.\n");
            }
            break;
        }
        case 5:
            showDriverStatistics(*driverHead);
            break;
        case 6:
        {
            int count = getIntegerInput("Show top how many drivers? (1-10): ", 1, 10);
            showTopDrivers(*driverHead, count);
            break;
        }
        case 7:
            running = 0;
            break;
        }
        if (running)
            pressEnterToContinue();
    }
}

static void adminInvoiceMenu(Invoice **invoiceHead)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Admin Invoice Management ---\n");
        printf("1. View All Invoices\n");
        printf("2. View Invoices by Status\n");
        printf("3. View Invoices by Customer\n");
        printf("4. Invoice Statistics\n");
        printf("5. Payment Method Statistics\n");
        printf("6. Back\n");
        int choice = getIntegerInput("Choose: ", 1, 6);

        switch (choice)
        {
        case 1:
            listAllInvoices(*invoiceHead);
            break;
        case 2:
        {
            printf("\nSelect Status:\n");
            printf("1. Pending\n");
            printf("2. Paid\n");
            printf("3. Cancelled\n");
            printf("4. Refunded\n");
            int statusChoice = getIntegerInput("Choose (1-4): ", 1, 4);
            InvoiceStatus status = statusChoice - 1;
            listInvoicesByStatus(*invoiceHead, status);
            break;
        }
        case 3:
        {
            int customerId = getIntegerInput("Enter Customer ID: ", 0, 9999);
            listInvoicesByCustomer(*invoiceHead, customerId);
            break;
        }
        case 4:
            showInvoiceStatistics(*invoiceHead);
            break;
        case 5:
            showPaymentMethodStats(*invoiceHead);
            break;
        case 6:
            running = 0;
            break;
        }
        if (running)
            pressEnterToContinue();
    }
}
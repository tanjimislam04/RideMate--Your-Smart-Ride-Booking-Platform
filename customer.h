#ifndef CUSTOMER_H
#define CUSTOMER_H

typedef struct Customer
{
    int id;
    char name[50];
    char username[30];
    char password[30];
    char email[100];
    char phone[20];
    int active; // for soft delete (1=active, 0=inactive)
    struct Customer *next;
} Customer;

// Core functions
void loadCustomers(Customer **head);
void saveCustomers(Customer *head);

Customer *authenticateCustomer(Customer *head, const char *username, const char *password);
void registerCustomer(Customer **head);
void displayCustomer(const Customer *c);
void updateCustomerProfile(Customer *c);

void adminCustomerMenu(Customer **head);
void displayAllCustomers(Customer *head);

#endif

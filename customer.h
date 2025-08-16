#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "utils.h"

typedef struct CustomerNode
{
    int id;
    char name[MAX_STRING];
    char username[MAX_STRING];
    char password[17]; // For hashed password
    char email[MAX_STRING];
    char phone[20];
    int active;
    struct CustomerNode *next;
} Customer;

void loadCustomers(Customer **head);
void saveCustomers(Customer *head);
void freeCustomerList(Customer **head);
Customer *findCustomerByUsername(Customer *head, const char *username);
Customer *authenticateCustomer(Customer *head, const char *username, const char *password);
void registerCustomer(Customer **head);
void displayCustomerProfile(const Customer *c);
void updateCustomerProfile(Customer *c);
void adminCustomerMenu(Customer **head);

#endif // CUSTOMER_H
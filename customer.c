#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customer.h"
#include "utils.h"

#define CUSTOMER_FILE "customers.csv"

Customer *parseCustomerCSV(char *line)
{
    Customer *c = (Customer *)malloc(sizeof(Customer));
    if (!c)
        return NULL;
    c->next = NULL;
    sscanf(line, "%d,%49[^,],%29[^,],%29[^,],%99[^,],%19[^,],%d",
           &c->id, c->name, c->username, c->password, c->email, c->phone, &c->active);
    return c;
}

void loadCustomers(Customer **head)
{
    FILE *f = fopen(CUSTOMER_FILE, "r");
    if (!f)
    {
        *head = NULL;
        return;
    }
    // Free old list if any
    while (*head)
    {
        Customer *tmp = *head;
        *head = (*head)->next;
        free(tmp);
    }
    char line[512];
    fgets(line, sizeof(line), f); // Skip header
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        Customer *c = parseCustomerCSV(line);
        if (c)
        {
            c->next = *head;
            *head = c;
        }
    }
    fclose(f);
}

void saveCustomers(Customer *head)
{
    FILE *f = fopen(CUSTOMER_FILE, "w");
    if (!f)
        return;
    fprintf(f, "id,name,username,password,email,phone,active\n");
    for (Customer *c = head; c; c = c->next)
    {
        fprintf(f, "%d,%s,%s,%s,%s,%s,%d\n",
                c->id, c->name, c->username, c->password, c->email, c->phone, c->active);
    }
    fclose(f);
}

Customer *findCustomerByUsername(Customer *head, const char *username)
{
    for (Customer *c = head; c; c = c->next)
    {
        if (c->active && strcmp(c->username, username) == 0)
        {
            return c;
        }
    }
    return NULL; // Move outside the loop!
}

Customer *authenticateCustomer(Customer *head, const char *username, const char *password)
{
    Customer *c = findCustomerByUsername(head, username);
    if (c && strcmp(c->password, password) == 0)
    {
        return c;
    }
    return NULL;
}

void registerCustomer(Customer **head)
{
    Customer *newCustomer = (Customer *)malloc(sizeof(Customer));
    if (!newCustomer)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    newCustomer->id = generateUniqueId();
    newCustomer->active = 1;
    newCustomer->next = NULL;

    while (1)
    {
        getInput("Enter your name: ", newCustomer->name, sizeof(newCustomer->name));
        int namelen = strlen(newCustomer->name);
        if (namelen == 0)
        {
            printf("Name cannot be empty. Please try again.\n");
            continue;
        }
        else if (namelen < 2)
        {
            printf("Name too short! Please try again.\n");
            continue;
        }
        break;
    }

    while (1)
    {
        getInput("Enter a unique username: ", newCustomer->username, sizeof(newCustomer->username));
        if (strlen(newCustomer->username) < 3)
        {
            printf("Username must be at least 3 characters long. Please try again.\n");
            continue;
        }
        if (findCustomerByUsername(*head, newCustomer->username))
        {
            printf("Username already exists. Please choose a different username.\n");
            continue;
        }
        break;
    }

    while (1)
    {
        getInput("Enter your password: ", newCustomer->password, sizeof(newCustomer->password));
        if (strlen(newCustomer->password) < 6)
        {
            printf("Password must be at least 6 characters long. Please try again.\n");
            continue;
        }
        if (strpbrk(newCustomer->password, "0123456789") == NULL)
        {
            printf("Password must contain at least one digit. Please try again.\n");
            continue;
        }
        if (strpbrk(newCustomer->password, "!@#$%^&*()_+") == NULL)
        {
            printf("Password must contain at least one special character. Please try again.\n");
            continue;
        }
        break;
    }

    while (1)
    {
        getInput("Enter your email: ", newCustomer->email, sizeof(newCustomer->email));
        if (!isValidEmail(newCustomer->email))
        {
            printf("Invalid email format. Please try again.\n");
            continue;
        }
        break;
    }

    while (1)
    {
        getInput("Enter your phone number(+880XXXXXXXXXX): ", newCustomer->phone, sizeof(newCustomer->phone));
        if (!isValidPhone(newCustomer->phone))
        {
            printf("Invalid phone number format. Must try with +880. Please try again\n");
            continue;
        }
        break;
    }

    newCustomer->next = *head;
    *head = newCustomer;
    saveCustomers(*head);
    printf("Registration successful! Your ID is %d\n", newCustomer->id);
}

void displayCustomer(const Customer *c)
{
    if (!c)
    {
        printf("No customer data available.\n");
        return;
    }
    printf("\n=== Customer Profile ===\n");
    printf("ID: %d\nName: %s\nUsername: %s\nEmail: %s\nPhone: %s\nStatus: %s\n",
           c->id, c->name, c->username, c->email, c->phone,
           c->active ? "Active" : "Inactive");
    printf("========================\n");
}

void updateCustomerProfile(Customer *c)
{
    if (!c)
    {
        printf("No customer data available to update.\n");
        return;
    }
    printf("\n=== Update Customer Profile (Leave blank to keep old) ===\n");

    char buf[100];
    getInput("New name: ", buf, sizeof(buf));
    if (strlen(buf))
        strcpy(c->name, buf);

    getInput("New email: ", buf, sizeof(buf));
    if (strlen(buf) && isValidEmail(buf))
        strcpy(c->email, buf);

    getInput("New phone: ", buf, sizeof(buf));
    if (strlen(buf) && isValidPhone(buf))
        strcpy(c->phone, buf);

    getInput("New password: ", buf, sizeof(buf));
    if (strlen(buf) && strlen(buf) > 3)
        strcpy(c->password, buf);

    printf("Profile updated.\n");
}

void displayAllCustomers(Customer *head)
{
    if (!head)
    {
        printf("No customers found.\n");
        return;
    }
    printf("\n=== All Customers ===\n");
    for (Customer *c = head; c; c = c->next)
    {
        if (c->active)
            displayCustomer(c);
    }
}

void adminCustomerMenu(Customer **head)
{
    int option;
    while (1)
    {
        printf("\n--- Admin Customer Menu ---\n");
        printf("1. View all customers\n2. Soft delete customer\n3. Export Customers CSV\n4. Import Customers CSV\n5. Back\nChoose: ");
        char input[10];
        getInput("", input, sizeof(input));
        option = atoi(input);
        if (option == 1)
        {
            displayAllCustomers(*head);
        }
        else if (option == 2)
        {
            int cid;
            printf("Enter Customer ID to soft delete: ");
            scanf("%d", &cid);
            getchar();
            for (Customer *c = *head; c; c = c->next)
            {
                if (c->id == cid && c->active)
                {
                    c->active = 0;
                    printf("Customer soft deleted.\n");
                    saveCustomers(*head);
                    break;
                }
            }
        }
        else if (option == 3)
        {
            saveCustomers(*head);
            printf("Exported to customers.csv\n");
        }
        else if (option == 4)
        {
            // Free old list, then import
            while (*head)
            {
                Customer *tmp = *head;
                *head = (*head)->next;
                free(tmp);
            }
            *head = NULL;
            loadCustomers(head);
            printf("Imported from customers.csv\n");
        }
        else if (option == 5)
        {
            break;
        }
        else
        {
            printf("Invalid!\n");
        }
    }
}

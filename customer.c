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
    while (*head)
    {
        Customer *tmp = *head;
        *head = (*head)->next;
        free(tmp);
    }
    char line[512];
    fgets(line, sizeof(line), f);
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
    return NULL;
}

Customer *authenticateCustomer(Customer *head, const char *username, const char *password)
{
    Customer *c = findCustomerByUsername(head, username);
    if (c)
    {
        char hashed_input[17];
        hash_password(password, hashed_input, sizeof(hashed_input));

        if (strcmp(c->password, hashed_input) == 0)
        {
            return c;
        }

        if (strcmp(c->password, password) == 0)
        {
            hash_password(password, c->password, sizeof(c->password));
            saveCustomers(head);
            return c;
        }
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
        if (!isValidName(newCustomer->name))
        {
            printf("Invalid name! Name must be at least 2 characters long, contain only letters and spaces.\n");
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
        char tempPassword[17];
        getInput("Enter your password: ", tempPassword, sizeof(tempPassword));
        if (strlen(tempPassword) < 6)
        {
            printf("Password must be at least 6 characters long. Please try again.\n");
            continue;
        }
        if (strpbrk(tempPassword, "0123456789") == NULL)
        {
            printf("Password must contain at least one digit. Please try again.\n");
            continue;
        }
        if (strpbrk(tempPassword, "!@#$%^&*()_+") == NULL)
        {
            printf("Password must contain at least one special character. Please try again.\n");
            continue;
        }
        hash_password(tempPassword, newCustomer->password, sizeof(newCustomer->password));
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
    {
        if (isValidName(buf))
        {
            strcpy(c->name, buf);
        }
        else
        {
            printf("Invalid name format! Name must contain only letters and spaces.\n");
        }
    }

    getInput("New email: ", buf, sizeof(buf));
    if (strlen(buf) && isValidEmail(buf))
        strcpy(c->email, buf);

    getInput("New phone: ", buf, sizeof(buf));
    if (strlen(buf) && isValidPhone(buf))
        strcpy(c->phone, buf);

    getInput("New password: ", buf, sizeof(buf));
    if (strlen(buf) && strlen(buf) > 3)
    {
        hash_password(buf, c->password, sizeof(c->password));
    }

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

void freeCustomerList(Customer **head)
{
    Customer *current = *head;
    Customer *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}

void displayCustomerProfile(const Customer *c)
{
    if (!c)
    {
        printf("Customer not found.\n");
        return;
    }

    printf("\n=== Customer Profile ===\n");
    printf("ID: %d\n", c->id);
    printf("Name: %s\n", c->name);
    printf("Username: %s\n", c->username);
    printf("Email: %s\n", c->email);
    printf("Phone: %s\n", c->phone);
    printf("Status: %s\n", c->active ? "Active" : "Inactive");
}

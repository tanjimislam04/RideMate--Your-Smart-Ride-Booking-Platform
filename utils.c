#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void getStringInput(const char *prompt, char *buffer, int size)
{
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

int getIntegerInput(const char *prompt, int min, int max)
{
    char buffer[100];
    int value;
    while (1)
    {
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &value) == 1 && value >= min && value <= max)
        {
            return value;
        }
        printf("Invalid input. Please enter a number between %d and %d.\n", min, max);
    }
}

float getFloatInput(const char *prompt, float min, float max)
{
    char buffer[100];
    float value;
    while (1)
    {
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%f", &value) == 1 && value >= min && value <= max)
        {
            return value;
        }
        printf("Invalid input. Please enter a number between %.2f and %.2f.\n", min, max);
    }
}

void pressEnterToContinue()
{
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int isValidEmail(const char *email)
{
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');
    if (!at || !dot || at > dot)
        return 0;
    return 1;
}

int isValidPhone(const char *phone)
{
    if (strlen(phone) != 14 || strncmp(phone, "+880", 4) != 0)
        return 0;
    for (int i = 4; phone[i] != '\0'; i++)
    {
        if (!isdigit(phone[i]))
            return 0;
    }
    return 1;
}

unsigned long hash_string(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

int authenticateAdmin(const char *username, const char *password)
{
    return (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0);
}

int generateUniqueId(void)
{
    static int id = 1000;
    return ++id;
}

void getInput(const char *prompt, char *buffer, int size)
{
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

int isValidNumber(const char *str)
{
    if (!str || strlen(str) == 0)
        return 0;
    
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}
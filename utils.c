#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void getStringInput(const char *prompt, char *buffer, int size)
{
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0;
}

int getIntegerInput(const char *prompt, int min, int max)
{
    char buffer[100];
    int value;
    while (1)
    {
        printf("%s", prompt);
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            continue;
        }
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
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            continue;
        }
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
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    fflush(stdin);
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
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

int isValidName(const char *name)
{
    if (!name || strlen(name) < 2)
        return 0;

    int hasLetter = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        if (isalpha(name[i]))
        {
            hasLetter = 1;
        }
        else if (name[i] != ' ')
        {
            return 0;
        }
    }
    return hasLetter;
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

void hash_to_string(unsigned long hash, char *hash_str, size_t size)
{
    snprintf(hash_str, size, "%lu", hash);
}

void hash_password(const char *password, char *hashed_password, size_t size)
{
    unsigned long hash = hash_string(password);
    hash_to_string(hash, hashed_password, size);
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

int stringToTime(const char *dateStr, time_t *outTime)
{
    struct tm tm = {0};
    int hour = 0, minute = 0;
    
    // Try datetime format first: "YYYY-MM-DD HH:MM"
    if (sscanf(dateStr, "%d-%d-%d %d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &hour, &minute) >= 3)
    {
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        tm.tm_hour = hour;
        tm.tm_min = minute;
        *outTime = mktime(&tm);
        return (*outTime != -1);
    }
    
    // Fallback to date-only format: "YYYY-MM-DD"
    if (sscanf(dateStr, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) == 3)
    {
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        *outTime = mktime(&tm);
        return (*outTime != -1);
    }
    
    return 0;
}
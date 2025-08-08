#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

static int uniqueIdCounter = 1001; // Starting ID for unique ID generation

void trimNewline(char *str)
{
    if (!str)
        return;
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

void getInput(const char *prompt, char *dest, int size)
{
    printf("%s", prompt);
    if (fgets(dest, size, stdin))
    {
        trimNewline(dest);
    }
    else
    {
        // fallback in case of error
        dest[0] = '\0';
    }
}

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        // Clear the input buffer
    }
}

int isValidEmail(const char *email)
{
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');
    if (!at || !dot)
        return 0;
    if (at > dot)
        return 0; // '@' must come before '.'
    if (at == email || dot == email + strlen(email) - 1)
        return 0; // '@' cannot be first, '.' cannot be last
    return 1;
}

int isValidPhone(const char *phone)
{
    int len = strlen(phone);
    if (len != 14)
        return 0; // Phone must be exactly 14 characters (e.g., +8801234567890)
    if (strncmp(phone, "+880", 4) != 0)
        return 0; // Must start with +880

    for (int i = 4; i < len; i++)
    {
        if (!isdigit(phone[i]))
            return 0; // All characters after +880 must be digits
    }
    return 1;
}

int isValidNumber(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
            return 0; // All characters must be digits
    }
    return 1;
}

int generateUniqueId()
{
    return uniqueIdCounter++;
}

int fileExists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return 1; // File exists
    }
    return 0; // File does not exist
}

// Admin authentication (simple hardcoded for core version)
int authenticateAdmin(const char *username, const char *password)
{
    /* Simple hardcoded creds for core MVP; replace with hashed storage later */
    return (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0);
}
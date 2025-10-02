#ifndef UTILS_H
#define UTILS_H

#include <time.h>

#define MAX_STRING 50

void getStringInput(const char *prompt, char *buffer, int size);
int getIntegerInput(const char *prompt, int min, int max);
float getFloatInput(const char *prompt, float min, float max);
void pressEnterToContinue();
void clearScreen();
void clearInputBuffer();
int isValidEmail(const char *email);
int isValidPhone(const char *phone);
int isValidName(const char *name);
unsigned long hash_string(const char *str);
void hash_to_string(unsigned long hash, char *hash_str, size_t size);
void hash_password(const char *password, char *hashed_password, size_t size);
int authenticateAdmin(const char *username, const char *password);
int generateUniqueId(void);
void getInput(const char *prompt, char *buffer, int size);
int isValidNumber(const char *str);

// Converts a "YYYY-MM-DD" string to a time_t value. Returns 1 on success, 0 on failure.
int stringToTime(const char *dateStr, time_t *outTime);

#endif // UTILS_H
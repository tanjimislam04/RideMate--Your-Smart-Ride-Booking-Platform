#ifndef UTILS_H
#define UTILS_H

#define MAX_STRING 50

void getStringInput(const char *prompt, char *buffer, int size);
int getIntegerInput(const char *prompt, int min, int max);
float getFloatInput(const char *prompt, float min, float max);
void pressEnterToContinue();
void clearScreen();
int isValidEmail(const char *email);
int isValidPhone(const char *phone);
unsigned long hash_string(const char *str);
int authenticateAdmin(const char *username, const char *password);
int generateUniqueId(void);
void getInput(const char *prompt, char *buffer, int size);
int isValidNumber(const char *str);

#endif // UTILS_H
#ifndef UTILS_H
#define UTILS_H

// Input validation

// Removes leading and trailing whitespace from a string
void trimNewline(char *str);

// Returns 1 if email format is valid, otherwise 0
int isValidEmail(const char *email);

// Returns 1 if phone is valid (length + digits), otherwise 0
int isValidPhone(const char *phone);

// Returns 1 if the string is a valid number, otherwise 0
int isValidNumber(const char *str);

// Prompt and take string input safely (removes newline)
void getInput(const char *prompt, char *dest, int size);

// Clears the input buffer to avoid issues with leftover characters
void clearInputBuffer();

// String utilities

// Unique ID generator (for core version, simple counter-based)
int generateUniqueId();

// File exists check
int fileExists(const char *filename);

// Admin authentication
int authenticateAdmin(const char *username, const char *password);

#endif

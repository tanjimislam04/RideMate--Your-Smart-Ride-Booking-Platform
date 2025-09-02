#include "backup.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int copyFile(const char *sourcePath, const char *destPath)
{
    FILE *source = fopen(sourcePath, "rb");
    if (!source)
        return 0;

    FILE *dest = fopen(destPath, "wb");
    if (!dest)
    {
        fclose(source);
        return 0;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0)
    {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(source);
    fclose(dest);
    return 1;
}

const char *dataFilesToBackup[] = {
    "data/customers.csv",
    "data/vehicles.csv",
    "data/routes.csv",
    "data/rentals.csv",
    "data/promos.csv",
    "data/drivers.csv"};
const int NUM_DATA_FILES = sizeof(dataFilesToBackup) / sizeof(dataFilesToBackup[0]);

static void createFullBackup()
{
    printf("\nCreating a full data backup...\n");

#ifdef _WIN32
    system("mkdir backups > nul 2>&1");
#else
    system("mkdir -p backups");
#endif

    char backupDir[100];
    time_t now = time(NULL);
    strftime(backupDir, sizeof(backupDir), "backups/backup_%Y-%m-%d_%H-%M-%S", localtime(&now));

    char cmd[120];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "mkdir %s > nul 2>&1", backupDir);
#else
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", backupDir);
#endif
    system(cmd);

    int success_count = 0;
    for (int i = 0; i < NUM_DATA_FILES; i++)
    {
        const char *sourceFile = dataFilesToBackup[i];

        const char *filename = strrchr(sourceFile, '/');
        if (!filename)
            filename = sourceFile;
        else
            filename++;

        char destFile[256];
        snprintf(destFile, sizeof(destFile), "%s/%s", backupDir, filename);

        if (copyFile(sourceFile, destFile))
        {
            printf("  - Backed up '%s'\n", sourceFile);
            success_count++;
        }
        else
        {
            printf("  - FAILED to back up '%s' (file might not exist yet)\n", sourceFile);
        }
    }

    if (success_count > 0)
    {
        printf("\nBackup completed! Files are saved in '%s'\n", backupDir);
    }
    else
    {
        printf("\nBackup failed. No data files were found to back up.\n");
    }
}

static void restoreFromBackup()
{
    printf("\n--- Restore Data from Backup ---\n");
    printf("WARNING: This will OVERWRITE your current data. This action cannot be undone.\n");

    char backupDirName[100];
    getStringInput("Enter the exact name of the backup folder to restore from: ", backupDirName, 100);

    char backupPath[120];
    snprintf(backupPath, sizeof(backupPath), "backups/%s", backupDirName);

    char checkFile[256];
    snprintf(checkFile, sizeof(checkFile), "%s/customers.csv", backupPath);
    FILE *f = fopen(checkFile, "r");
    if (!f)
    {
        printf("Error: Backup folder '%s' not found or is invalid.\n", backupDirName);
        return;
    }
    fclose(f);

    printf("Are you absolutely sure you want to restore from '%s'? (y/n): ", backupDirName);
    char confirm[10];
    getStringInput("", confirm, 10);

    if (confirm[0] == 'y' || confirm[0] == 'Y')
    {
        int success_count = 0;
        for (int i = 0; i < NUM_DATA_FILES; i++)
        {
            const char *destFile = dataFilesToBackup[i];

            const char *filename = strrchr(destFile, '/');
            if (!filename)
                filename = destFile;
            else
                filename++;

            char sourceFile[256];
            snprintf(sourceFile, sizeof(sourceFile), "%s/%s", backupPath, filename);

            if (copyFile(sourceFile, destFile))
            {
                printf("  - Restored '%s'\n", destFile);
                success_count++;
            }
            else
            {
                printf("  - FAILED to restore '%s' (file missing in backup)\n", destFile);
            }
        }

        if (success_count > 0)
        {
            printf("\nRestore complete. Please RESTART the application to load the restored data.\n");
        }
        else
        {
            printf("\nRestore failed. No files were copied.\n");
        }
    }
    else
    {
        printf("Restore cancelled.\n");
    }
}

void adminBackupMenu()
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n--- Data Management (Backup & Restore) ---\n");
        printf("1. Create Full Backup of All Data\n");
        printf("2. Restore Data from a Backup\n");
        printf("3. Back to Admin Panel\n");
        int choice = getIntegerInput("Enter choice: ", 1, 3);

        switch (choice)
        {
        case 1:
            createFullBackup();
            break;
        case 2:
            restoreFromBackup();
            break;
        case 3:
            running = 0;
            break;
        }
        if (running)
            pressEnterToContinue();
    }
}

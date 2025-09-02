#include "complaint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COMPLAINT_FILE "complaints.csv"

static int nextComplaintId = 7001;

// Utility Functions
const char *complaintStatusStr(ComplaintStatus status)
{
    switch (status)
    {
    case COMPLAINT_PENDING:
        return "PENDING";
    case COMPLAINT_IN_PROGRESS:
        return "IN PROGRESS";
    case COMPLAINT_RESOLVED:
        return "RESOLVED";
    case COMPLAINT_CLOSED:
        return "CLOSED";
    default:
        return "UNKNOWN";
    }
}

void formatTimeString(time_t timestamp, char *buffer, size_t size)
{
    struct tm *tm_info = localtime(&timestamp);
    strftime(buffer, size, "%Y-%m-%d %H:%M", tm_info);
}

static void adjustNextComplaintId(Complaint *head)
{
    int maxId = 7000;
    for (Complaint *c = head; c; c = c->next)
        if (c->id > maxId)
            maxId = c->id;
    nextComplaintId = maxId + 1;
}

static Complaint *parseComplaintCSV(char *line)
{
    Complaint *c = (Complaint *)malloc(sizeof(Complaint));
    if (!c)
        return NULL;
    
    int status;
    long createdAt, resolvedAt;
    
    int result = sscanf(line, "%d,%d,%d,%200[^,],%200[^,],%d,%ld,%ld",
                       &c->id, &c->rentalId, &c->customerId, c->description,
                       c->adminResponse, &status, &createdAt, &resolvedAt);
    
    if (result != 8) {
        printf("Warning: Failed to parse complaint line: %s (parsed %d fields)\n", line, result);
        free(c);
        return NULL;
    }
    
    c->status = (ComplaintStatus)status;
    c->createdAt = (time_t)createdAt;
    c->resolvedAt = (time_t)resolvedAt;
    c->next = NULL;
    
    return c;
}

// CSV I/O Functions
void loadComplaints(Complaint **head)
{
    *head = NULL;
    FILE *f = fopen(COMPLAINT_FILE, "r");
    if (!f)
        return;

    char line[512];
    if (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "id,", 3) != 0)
        {
            // If no header, rewind to start
            rewind(f);
        }
    }

    int count = 0;
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        Complaint *c = parseComplaintCSV(line);
        if (c)
        {
            c->next = *head;
            *head = c;
            count++;
        }
    }
    fclose(f);
    adjustNextComplaintId(*head);
    printf("Loaded %d complaints from %s\n", count, COMPLAINT_FILE);
}

void saveComplaints(Complaint *head)
{
    FILE *f = fopen(COMPLAINT_FILE, "w");
    if (!f)
    {
        printf("Error: Could not open file %s for writing\n", COMPLAINT_FILE);
        return;
    }
    
    fprintf(f, "id,rentalId,customerId,description,adminResponse,status,createdAt,resolvedAt\n");
    int count = 0;
    for (Complaint *c = head; c; c = c->next)
    {
        fprintf(f, "%d,%d,%d,%s,%s,%d,%ld,%ld\n",
                c->id, c->rentalId, c->customerId, c->description,
                c->adminResponse, (int)c->status, (long)c->createdAt, (long)c->resolvedAt);
        count++;
    }
    fclose(f);
    printf("Successfully saved %d complaints to %s\n", count, COMPLAINT_FILE);
}

void freeComplaintList(Complaint **head)
{
    Complaint *current = *head;
    Complaint *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}

// Search Functions
Complaint *findComplaintById(Complaint *head, int complaintId)
{
    for (Complaint *c = head; c; c = c->next)
        if (c->id == complaintId)
            return c;
    return NULL;
}

Complaint *findComplaintsByRentalId(Complaint *head, int rentalId)
{
    for (Complaint *c = head; c; c = c->next)
        if (c->rentalId == rentalId)
            return c;
    return NULL;
}

Complaint *findComplaintsByCustomerId(Complaint *head, int customerId)
{
    for (Complaint *c = head; c; c = c->next)
        if (c->customerId == customerId)
            return c;
    return NULL;
}

// Customer Functions
void fileComplaint(Complaint **head, int rentalId, int customerId)
{
    Complaint *c = (Complaint *)malloc(sizeof(Complaint));
    if (!c)
    {
        printf("Error: Memory allocation failed\n");
        return;
    }
    
    c->id = nextComplaintId++;
    c->rentalId = rentalId;
    c->customerId = customerId;
    c->status = COMPLAINT_PENDING;
    c->createdAt = time(NULL);
    c->resolvedAt = 0;
    c->adminResponse[0] = '\0';
    
    printf("\n=== File a Complaint ===\n");
    printf("Please describe your complaint (max 200 characters):\n");
    getStringInput("Description: ", c->description, sizeof(c->description));
    
    if (strlen(c->description) < 10)
    {
        printf("Error: Description must be at least 10 characters long.\n");
        free(c);
        return;
    }
    
    c->next = *head;
    *head = c;
    saveComplaints(*head);
    
    printf("\nComplaint filed successfully!\n");
    printf("Complaint ID: %d | Status: %s\n", c->id, complaintStatusStr(c->status));
}

void viewCustomerComplaints(Complaint *head, int customerId)
{
    printf("\n=== Your Complaints ===\n");
    int found = 0;
    
    for (Complaint *c = head; c; c = c->next)
    {
        if (c->customerId == customerId)
        {
            displayComplaint(c);
            found = 1;
        }
    }
    
    if (!found)
    {
        printf("No complaints found for your account.\n");
    }
}

// Display Functions
void displayComplaint(const Complaint *c)
{
    if (!c)
        return;
        
    char createdAtStr[32], resolvedAtStr[32];
    formatTimeString(c->createdAt, createdAtStr, sizeof(createdAtStr));
    
    printf("\nComplaint #%d (Rental #%d)\n", c->id, c->rentalId);
    printf("Status: %s\n", complaintStatusStr(c->status));
    printf("Filed: %s\n", createdAtStr);
    printf("Description: %s\n", c->description);
    
    if (c->adminResponse[0] != '\0')
    {
        printf("Admin Response: %s\n", c->adminResponse);
    }
    
    if (c->resolvedAt > 0)
    {
        formatTimeString(c->resolvedAt, resolvedAtStr, sizeof(resolvedAtStr));
        printf("Resolved: %s\n", resolvedAtStr);
    }
    
    printf("----------------------------------------\n");
}

void displayComplaintWithRentalInfo(const Complaint *c, Rental *rentalHead)
{
    if (!c)
        return;
        
    displayComplaint(c);
    
    // Find and display rental information
    Rental *rental = findRentalById(rentalHead, c->rentalId);
    if (rental)
    {
        printf("Rental Details:\n");
        printf("  Vehicle ID: %d\n", rental->vehicleId);
        printf("  Type: %s\n", rental->type == RENT_HOURLY ? "Hourly" : 
                               rental->type == RENT_DAILY ? "Daily" : "Route");
        printf("  Start: %s | End: %s\n", rental->startTime, rental->endTime);
        printf("  Cost: $%.2f\n", rental->totalCost);
    }
    else
    {
        printf("Rental #%d not found.\n", c->rentalId);
    }
}

// Admin Functions
void adminComplaintMenu(Complaint **head, Rental *rentalHead)
{
    int running = 1;
    while (running)
    {
        clearScreen();
        printf("\n=== Complaint Management ===\n");
        printf("1. View All Complaints\n");
        printf("2. View Complaints by Status\n");
        printf("3. Respond to Complaint\n");
        printf("4. Update Complaint Status\n");
        printf("5. Complaint Statistics\n");
        printf("6. Back to Main Menu\n");
        
        int choice = getIntegerInput("Enter your choice: ", 1, 6);
        
        switch (choice)
        {
        case 1:
            listAllComplaints(*head);
            break;
        case 2:
        {
            printf("\nSelect Status:\n");
            printf("1. Pending\n");
            printf("2. In Progress\n");
            printf("3. Resolved\n");
            printf("4. Closed\n");
            int statusChoice = getIntegerInput("Choose (1-4): ", 1, 4);
            ComplaintStatus status = statusChoice - 1;
            listComplaintsByStatus(*head, status);
            break;
        }
        case 3:
            respondToComplaint(*head, rentalHead);
            break;
        case 4:
            updateComplaintStatus(*head);
            break;
        case 5:
            showComplaintStatistics(*head);
            break;
        case 6:
            running = 0;
            break;
        }
        
        if (running)
            pressEnterToContinue();
    }
}

void listAllComplaints(Complaint *head)
{
    if (!head)
    {
        printf("No complaints found.\n");
        return;
    }
    
    printf("\n=== All Complaints ===\n");
    int count = 0;
    for (Complaint *c = head; c; c = c->next)
    {
        displayComplaint(c);
        count++;
    }
    printf("\nTotal complaints: %d\n", count);
}

void listComplaintsByStatus(Complaint *head, ComplaintStatus status)
{
    printf("\n=== Complaints - %s ===\n", complaintStatusStr(status));
    int count = 0;
    
    for (Complaint *c = head; c; c = c->next)
    {
        if (c->status == status)
        {
            displayComplaint(c);
            count++;
        }
    }
    
    if (count == 0)
    {
        printf("No complaints with status '%s' found.\n", complaintStatusStr(status));
    }
    else
    {
        printf("\nTotal complaints with status '%s': %d\n", complaintStatusStr(status), count);
    }
}

void respondToComplaint(Complaint *head, Rental *rentalHead)
{
    if (!head)
    {
        printf("No complaints to respond to.\n");
        return;
    }
    
    listAllComplaints(head);
    int complaintId = getIntegerInput("\nEnter Complaint ID to respond to: ", 7001, 9999);
    
    Complaint *c = findComplaintById(head, complaintId);
    if (!c)
    {
        printf("Complaint not found.\n");
        return;
    }
    
    printf("\n=== Respond to Complaint #%d ===\n", c->id);
    displayComplaintWithRentalInfo(c, rentalHead);
    
    printf("\nEnter your response (max 200 characters):\n");
    getStringInput("Response: ", c->adminResponse, sizeof(c->adminResponse));
    
    if (strlen(c->adminResponse) < 5)
    {
        printf("Error: Response must be at least 5 characters long.\n");
        return;
    }
    
    // Update status to in progress if it was pending
    if (c->status == COMPLAINT_PENDING)
    {
        c->status = COMPLAINT_IN_PROGRESS;
    }
    
    saveComplaints(head);
    printf("\nResponse saved successfully!\n");
}

void updateComplaintStatus(Complaint *head)
{
    if (!head)
    {
        printf("No complaints to update.\n");
        return;
    }
    
    listAllComplaints(head);
    int complaintId = getIntegerInput("\nEnter Complaint ID to update: ", 7001, 9999);
    
    Complaint *c = findComplaintById(head, complaintId);
    if (!c)
    {
        printf("Complaint not found.\n");
        return;
    }
    
    printf("\nCurrent Status: %s\n", complaintStatusStr(c->status));
    printf("\nSelect New Status:\n");
    printf("1. Pending\n");
    printf("2. In Progress\n");
    printf("3. Resolved\n");
    printf("4. Closed\n");
    
    int statusChoice = getIntegerInput("Choose (1-4): ", 1, 4);
    ComplaintStatus newStatus = statusChoice - 1;
    
    c->status = newStatus;
    
    // Set resolved time if status is resolved or closed
    if (newStatus == COMPLAINT_RESOLVED || newStatus == COMPLAINT_CLOSED)
    {
        c->resolvedAt = time(NULL);
    }
    
    saveComplaints(head);
    printf("\nStatus updated successfully!\n");
}

void showComplaintStatistics(Complaint *head)
{
    if (!head)
    {
        printf("No complaints found.\n");
        return;
    }
    
    int total = 0, pending = 0, inProgress = 0, resolved = 0, closed = 0;
    
    for (Complaint *c = head; c; c = c->next)
    {
        total++;
        switch (c->status)
        {
        case COMPLAINT_PENDING:
            pending++;
            break;
        case COMPLAINT_IN_PROGRESS:
            inProgress++;
            break;
        case COMPLAINT_RESOLVED:
            resolved++;
            break;
        case COMPLAINT_CLOSED:
            closed++;
            break;
        }
    }
    
    printf("\n=== Complaint Statistics ===\n");
    printf("Total Complaints: %d\n", total);
    printf("Pending: %d (%.1f%%)\n", pending, total > 0 ? (float)pending/total*100 : 0);
    printf("In Progress: %d (%.1f%%)\n", inProgress, total > 0 ? (float)inProgress/total*100 : 0);
    printf("Resolved: %d (%.1f%%)\n", resolved, total > 0 ? (float)resolved/total*100 : 0);
    printf("Closed: %d (%.1f%%)\n", closed, total > 0 ? (float)closed/total*100 : 0);
    
    if (total > 0)
    {
        float resolutionRate = (float)(resolved + closed) / total * 100;
        printf("\nOverall Resolution Rate: %.1f%%\n", resolutionRate);
    }
}

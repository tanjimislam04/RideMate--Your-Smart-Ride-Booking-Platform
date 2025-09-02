#ifndef COMPLAINT_H
#define COMPLAINT_H

#include "utils.h"
#include "rental.h"

// Complaint Status Enum
typedef enum
{
    COMPLAINT_PENDING = 0,
    COMPLAINT_IN_PROGRESS = 1,
    COMPLAINT_RESOLVED = 2,
    COMPLAINT_CLOSED = 3
} ComplaintStatus;

// Complaint Structure
typedef struct ComplaintNode
{
    int id;
    int rentalId;
    int customerId;
    char description[201];  // Complaint description (max 200 chars + null terminator)
    char adminResponse[201]; // Admin response (max 200 chars + null terminator)
    ComplaintStatus status;
    time_t createdAt;
    time_t resolvedAt;
    struct ComplaintNode *next;
} Complaint;

// CSV I/O Functions
void loadComplaints(Complaint **head);
void saveComplaints(Complaint *head);
void freeComplaintList(Complaint **head);

// Search Functions
Complaint *findComplaintById(Complaint *head, int complaintId);
Complaint *findComplaintsByRentalId(Complaint *head, int rentalId);
Complaint *findComplaintsByCustomerId(Complaint *head, int customerId);

// Customer Functions
void fileComplaint(Complaint **head, int rentalId, int customerId);
void viewCustomerComplaints(Complaint *head, int customerId);

// Admin Functions
void adminComplaintMenu(Complaint **head, Rental *rentalHead);
void listAllComplaints(Complaint *head);
void listComplaintsByStatus(Complaint *head, ComplaintStatus status);
void respondToComplaint(Complaint *head, Rental *rentalHead);
void updateComplaintStatus(Complaint *head);
void showComplaintStatistics(Complaint *head);

// Display Functions
void displayComplaint(const Complaint *c);
void displayComplaintWithRentalInfo(const Complaint *c, Rental *rentalHead);

// Utility Functions
const char *complaintStatusStr(ComplaintStatus status);
void formatTimeString(time_t timestamp, char *buffer, size_t size);

#endif // COMPLAINT_H

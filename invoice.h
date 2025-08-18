#ifndef INVOICE_H
#define INVOICE_H

#include <time.h>

// --- Payment Method Enum ---
typedef enum
{
    PAYMENT_CASH = 0,
    PAYMENT_CARD = 1,
    PAYMENT_MOBILE_BANKING = 2,
    PAYMENT_CRYPTO = 3
} PaymentMethod;

// --- Invoice Status Enum ---
typedef enum
{
    INVOICE_PENDING = 0,
    INVOICE_PAID = 1,
    INVOICE_CANCELLED = 2,
    INVOICE_REFUNDED = 3
} InvoiceStatus;

// --- Invoice Struct ---
typedef struct InvoiceNode
{
    int id;                      // Unique invoice ID
    int rentalId;                // Associated rental ID
    int customerId;              // Customer ID
    int driverId;                // Driver ID (if assigned)
    float subtotal;              // Original cost before discounts
    float discountAmount;        // Discount amount applied
    float taxAmount;             // Tax amount
    float totalAmount;           // Final amount to pay
    PaymentMethod paymentMethod; // How payment was made
    InvoiceStatus status;        // Current invoice status
    char promoCode[20];          // Promo code used (if any)
    char paymentReference[50];   // Payment reference/transaction ID
    time_t createdAt;            // Invoice creation timestamp
    time_t paidAt;               // Payment timestamp
    struct InvoiceNode *next;    // Pointer to next invoice
} Invoice;

// --- Function Prototypes ---

// CSV I/O Functions
void loadInvoices(Invoice **head);
void saveInvoices(Invoice *head);

// Invoice Management Functions
Invoice *createInvoice(int rentalId, int customerId, int driverId, float subtotal,
                       float discountAmount, const char *promoCode);
void updateInvoiceStatus(Invoice *invoice, InvoiceStatus status, PaymentMethod method,
                         const char *paymentRef);
Invoice *findInvoiceById(Invoice *head, int invoiceId);
Invoice *findInvoiceByRentalId(Invoice *head, int rentalId);

// Receipt Generation Functions
void generateReceipt(Invoice *invoice, const char *customerName, const char *vehicleInfo,
                     const char *driverName);
void saveReceiptToFile(Invoice *invoice, const char *customerName, const char *vehicleInfo,
                       const char *driverName);
void displayInvoice(const Invoice *invoice);

// Invoice Listings
void listAllInvoices(Invoice *head);
void listInvoicesByCustomer(Invoice *head, int customerId);
void listInvoicesByStatus(Invoice *head, InvoiceStatus status);

// Payment Processing
int processPayment(Invoice *invoice, PaymentMethod method, const char *paymentRef);
void refundInvoice(Invoice *invoice, const char *reason);

// Invoice Statistics
void showInvoiceStatistics(Invoice *head);
void showPaymentMethodStats(Invoice *head);

// Memory Management
void freeInvoiceList(Invoice **head);

#endif // INVOICE_H

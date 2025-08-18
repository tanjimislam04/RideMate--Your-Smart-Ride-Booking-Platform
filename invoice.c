// File: invoice.c
// Description: Implements payment and invoicing system for RideMate

#include "invoice.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INVOICE_FILE "data/invoices.csv"
#define RECEIPTS_DIR "receipts"

// --- Helper Functions ---

static void ensureInvoiceFileExists()
{
    FILE *f = fopen(INVOICE_FILE, "r");
    if (f)
    {
        fclose(f);
        return;
    }

    f = fopen(INVOICE_FILE, "w");
    if (f)
    {
        fprintf(f, "id,rentalId,customerId,driverId,subtotal,discountAmount,taxAmount,totalAmount,paymentMethod,status,promoCode,paymentReference,createdAt,paidAt\n");
        fclose(f);
    }
}

static void ensureReceiptsDirectoryExists()
{
#ifdef _WIN32
    system("if not exist receipts mkdir receipts");
#else
    system("mkdir -p receipts");
#endif
}

static const char *paymentMethodStr(PaymentMethod method)
{
    switch (method)
    {
    case PAYMENT_CASH:
        return "Cash";
    case PAYMENT_CARD:
        return "Card";
    case PAYMENT_MOBILE_BANKING:
        return "Mobile Banking";
    case PAYMENT_CRYPTO:
        return "Cryptocurrency";
    default:
        return "Unknown";
    }
}

static const char *invoiceStatusStr(InvoiceStatus status)
{
    switch (status)
    {
    case INVOICE_PENDING:
        return "Pending";
    case INVOICE_PAID:
        return "Paid";
    case INVOICE_CANCELLED:
        return "Cancelled";
    case INVOICE_REFUNDED:
        return "Refunded";
    default:
        return "Unknown";
    }
}

// --- Invoice Management Functions ---

Invoice *createInvoice(int rentalId, int customerId, int driverId, float subtotal,
                       float discountAmount, const char *promoCode)
{
    Invoice *inv = (Invoice *)malloc(sizeof(Invoice));
    if (!inv)
        return NULL;

    static int nextId = 1001;
    inv->id = nextId++;
    inv->rentalId = rentalId;
    inv->customerId = customerId;
    inv->driverId = driverId;
    inv->subtotal = subtotal;
    inv->discountAmount = discountAmount;
    inv->taxAmount = subtotal * 0.15; // 15% tax
    inv->totalAmount = subtotal - discountAmount + inv->taxAmount;
    inv->paymentMethod = PAYMENT_CASH;
    inv->status = INVOICE_PENDING;
    strncpy(inv->promoCode, promoCode ? promoCode : "", sizeof(inv->promoCode) - 1);
    inv->promoCode[sizeof(inv->promoCode) - 1] = '\0';
    inv->paymentReference[0] = '\0';
    inv->createdAt = time(NULL);
    inv->paidAt = 0;
    inv->next = NULL;

    return inv;
}

void updateInvoiceStatus(Invoice *invoice, InvoiceStatus status, PaymentMethod method, const char *paymentRef)
{
    if (!invoice)
        return;

    invoice->status = status;
    invoice->paymentMethod = method;
    if (paymentRef)
    {
        strncpy(invoice->paymentReference, paymentRef, sizeof(invoice->paymentReference) - 1);
        invoice->paymentReference[sizeof(invoice->paymentReference) - 1] = '\0';
    }

    if (status == INVOICE_PAID)
    {
        invoice->paidAt = time(NULL);
    }
}

Invoice *findInvoiceById(Invoice *head, int invoiceId)
{
    for (Invoice *inv = head; inv; inv = inv->next)
    {
        if (inv->id == invoiceId)
            return inv;
    }
    return NULL;
}

Invoice *findInvoiceByRentalId(Invoice *head, int rentalId)
{
    for (Invoice *inv = head; inv; inv = inv->next)
    {
        if (inv->rentalId == rentalId)
            return inv;
    }
    return NULL;
}

// --- Receipt Generation Functions ---

void generateReceipt(Invoice *invoice, const char *customerName, const char *vehicleInfo,
                     const char *driverName)
{
    if (!invoice)
        return;

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    RIDEMATE - RECEIPT                       ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Invoice ID: %-45d ║\n", invoice->id);
    printf("║ Date: %-50s ║\n", ctime(&invoice->createdAt));
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ CUSTOMER INFORMATION                                         ║\n");
    printf("║ Name: %-50s ║\n", customerName);
    printf("║ Customer ID: %-45d ║\n", invoice->customerId);
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ RENTAL DETAILS                                               ║\n");
    printf("║ Rental ID: %-45d ║\n", invoice->rentalId);
    printf("║ Vehicle: %-50s ║\n", vehicleInfo);
    if (invoice->driverId > 0)
    {
        printf("║ Driver: %-50s ║\n", driverName ? driverName : "Assigned");
    }
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ PAYMENT BREAKDOWN                                            ║\n");
    printf("║ Subtotal: $%-47.2f ║\n", invoice->subtotal);
    if (invoice->discountAmount > 0)
    {
        printf("║ Discount (%s): -$%-42.2f ║\n", invoice->promoCode, invoice->discountAmount);
    }
    printf("║ Tax (15%%): $%-47.2f ║\n", invoice->taxAmount);
    printf("║ ─────────────────────────────────────────────────────────── ║\n");
    printf("║ TOTAL: $%-47.2f ║\n", invoice->totalAmount);
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ PAYMENT INFORMATION                                          ║\n");
    printf("║ Status: %-50s ║\n", invoiceStatusStr(invoice->status));
    printf("║ Method: %-50s ║\n", paymentMethodStr(invoice->paymentMethod));
    if (invoice->paymentReference[0])
    {
        printf("║ Reference: %-47s ║\n", invoice->paymentReference);
    }
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Thank you for choosing RideMate!                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}

void saveReceiptToFile(Invoice *invoice, const char *customerName, const char *vehicleInfo,
                       const char *driverName)
{
    if (!invoice)
        return;

    ensureReceiptsDirectoryExists();

    char filename[256];
    sprintf(filename, "%s/receipt_%d.txt", RECEIPTS_DIR, invoice->id);

    FILE *f = fopen(filename, "w");
    if (!f)
    {
        printf("Error: Could not create receipt file.\n");
        return;
    }

    fprintf(f, "RIDEMATE - RECEIPT\n");
    fprintf(f, "══════════════════════════════════════════════════════════════\n");
    fprintf(f, "Invoice ID: %d\n", invoice->id);
    fprintf(f, "Date: %s", ctime(&invoice->createdAt));
    fprintf(f, "\nCUSTOMER INFORMATION\n");
    fprintf(f, "Name: %s\n", customerName);
    fprintf(f, "Customer ID: %d\n", invoice->customerId);
    fprintf(f, "\nRENTAL DETAILS\n");
    fprintf(f, "Rental ID: %d\n", invoice->rentalId);
    fprintf(f, "Vehicle: %s\n", vehicleInfo);
    if (invoice->driverId > 0)
    {
        fprintf(f, "Driver: %s\n", driverName ? driverName : "Assigned");
    }
    fprintf(f, "\nPAYMENT BREAKDOWN\n");
    fprintf(f, "Subtotal: $%.2f\n", invoice->subtotal);
    if (invoice->discountAmount > 0)
    {
        fprintf(f, "Discount (%s): -$%.2f\n", invoice->promoCode, invoice->discountAmount);
    }
    fprintf(f, "Tax (15%%): $%.2f\n", invoice->taxAmount);
    fprintf(f, "──────────────────────────────────────────────────────────\n");
    fprintf(f, "TOTAL: $%.2f\n", invoice->totalAmount);
    fprintf(f, "\nPAYMENT INFORMATION\n");
    fprintf(f, "Status: %s\n", invoiceStatusStr(invoice->status));
    fprintf(f, "Method: %s\n", paymentMethodStr(invoice->paymentMethod));
    if (invoice->paymentReference[0])
    {
        fprintf(f, "Reference: %s\n", invoice->paymentReference);
    }
    fprintf(f, "\nThank you for choosing RideMate!\n");

    fclose(f);
    printf("\nReceipt saved to: %s\n", filename);
}

// --- Placeholder functions for compilation ---

void loadInvoices(Invoice **head) { *head = NULL; }
void saveInvoices(Invoice *head) {}
void displayInvoice(const Invoice *invoice) {}
void listAllInvoices(Invoice *head) {}
void listInvoicesByCustomer(Invoice *head, int customerId) {}
void listInvoicesByStatus(Invoice *head, InvoiceStatus status) {}
int processPayment(Invoice *invoice, PaymentMethod method, const char *paymentRef) { return 0; }
void refundInvoice(Invoice *invoice, const char *reason) {}
void showInvoiceStatistics(Invoice *head) {}
void showPaymentMethodStats(Invoice *head) {}
void freeInvoiceList(Invoice **head) {}

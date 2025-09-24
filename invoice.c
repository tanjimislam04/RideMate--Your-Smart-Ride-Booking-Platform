#include "invoice.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void ensureCsvWithHeader(const char *path, const char *headerLine)
{
    FILE *r = fopen(path, "r");
    if (r)
    {
        fclose(r);
        return;
    }
    FILE *w = fopen(path, "w");
    if (!w)
    {
        printf("Error: could not create %s\n", path);
        return;
    }
    fputs(headerLine, w);
    fputc('\n', w);
    fclose(w);
}

#define INVOICE_FILE "data/invoices.csv"
#define RECEIPTS_DIR "receipts"

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

Invoice *createInvoice(int rentalId, int customerId, int driverId, float subtotal,
                       float discountAmount, const char *promoCode)
{
    Invoice *inv = (Invoice *)malloc(sizeof(Invoice));
    if (!inv)
        return NULL;

    static int nextId = 6001;
    inv->id = nextId++;
    inv->rentalId = rentalId;
    inv->customerId = customerId;
    inv->driverId = driverId;
    inv->subtotal = subtotal;
    inv->discountAmount = discountAmount;
    inv->taxAmount = subtotal * 0.15;
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

static Invoice *parseInvoiceCSV(const char *line)
{
    Invoice *inv = (Invoice *)malloc(sizeof(Invoice));
    if (!inv)
        return NULL;

    int result = sscanf(line, "%d,%d,%d,%d,%f,%f,%f,%f,%d,%d,%[^,],%[^,],%ld",
                        &inv->id, &inv->customerId, &inv->rentalId, &inv->driverId,
                        &inv->subtotal, &inv->discountAmount, &inv->taxAmount, &inv->totalAmount,
                        (int*)&inv->status, (int*)&inv->paymentMethod, inv->paymentReference, inv->promoCode, (long*)&inv->createdAt);

    if (result < 13)
    {
        free(inv);
        return NULL;
    }

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

void generateReceipt(Invoice *invoice, const char *customerName, const char *vehicleInfo,
                     const char *driverName)
{
    if (!invoice)
        return;

    printf("\n");
    printf("================================================================\n");
    printf("                    RIDEMATE - RECEIPT                         \n");
    printf("================================================================\n");
    printf("Invoice ID: %d\n", invoice->id);
    printf("Date: %s", ctime(&invoice->createdAt));
    printf("================================================================\n");
    printf("CUSTOMER INFORMATION\n");
    printf("Name: %s\n", customerName);
    printf("Customer ID: %d\n", invoice->customerId);
    printf("================================================================\n");
    printf("RENTAL DETAILS\n");
    printf("Rental ID: %d\n", invoice->rentalId);
    printf("Vehicle: %s\n", vehicleInfo);
    if (invoice->driverId > 0)
    {
        printf("Driver: %s\n", driverName ? driverName : "Assigned");
    }
    printf("================================================================\n");
    printf("PAYMENT BREAKDOWN\n");
    printf("Subtotal: $%.2f\n", invoice->subtotal);
    if (invoice->discountAmount > 0)
    {
        printf("Discount (%s): -$%.2f\n", invoice->promoCode, invoice->discountAmount);
    }
    printf("Tax (15%%): $%.2f\n", invoice->taxAmount);
    printf("----------------------------------------------------------------\n");
    printf("TOTAL: $%.2f\n", invoice->totalAmount);
    printf("================================================================\n");
    printf("PAYMENT INFORMATION\n");
    printf("Status: %s\n", invoiceStatusStr(invoice->status));
    printf("Method: %s\n", paymentMethodStr(invoice->paymentMethod));
    if (invoice->paymentReference[0])
    {
        printf("Reference: %s\n", invoice->paymentReference);
    }
    printf("================================================================\n");
    printf("Thank you for choosing RideMate!\n");
    printf("================================================================\n");
}

void saveReceiptToFile(Invoice *invoice, const char *customerName, const char *vehicleInfo,
                       const char *driverName)
{
    if (!invoice)
        return;

    ensureReceiptsDirectoryExists();

    char filename[256];
    snprintf(filename, sizeof(filename), "%s/receipt_%d.txt", RECEIPTS_DIR, invoice->id);

    FILE *f = fopen(filename, "w");
    if (!f)
    {
        printf("Error: Could not create receipt file.\n");
        return;
    }

    fprintf(f, "================================================================\n");
    fprintf(f, "                    RIDEMATE - RECEIPT                          \n");
    fprintf(f, "================================================================\n");
    fprintf(f, "Invoice ID: %d\n", invoice->id);
    fprintf(f, "Date: %s", ctime(&invoice->createdAt));
    fprintf(f, "================================================================\n");
    fprintf(f, "CUSTOMER INFORMATION\n");
    fprintf(f, "Name: %s\n", customerName);
    fprintf(f, "Customer ID: %d\n", invoice->customerId);
    fprintf(f, "================================================================\n");
    fprintf(f, "RENTAL DETAILS\n");
    fprintf(f, "Rental ID: %d\n", invoice->rentalId);
    fprintf(f, "Vehicle: %s\n", vehicleInfo);
    if (invoice->driverId > 0)
    {
        fprintf(f, "Driver: %s\n", driverName ? driverName : "Assigned");
    }
    fprintf(f, "================================================================\n");
    fprintf(f, "PAYMENT BREAKDOWN\n");
    fprintf(f, "Subtotal: $%.2f\n", invoice->subtotal);
    if (invoice->discountAmount > 0)
    {
        fprintf(f, "Discount (%s): -$%.2f\n", invoice->promoCode, invoice->discountAmount);
    }
    fprintf(f, "Tax (15%%): $%.2f\n", invoice->taxAmount);
    fprintf(f, "----------------------------------------------------------------\n");
    fprintf(f, "TOTAL: $%.2f\n", invoice->totalAmount);
    fprintf(f, "================================================================\n");
    fprintf(f, "PAYMENT INFORMATION\n");
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

void loadInvoices(Invoice **head)
{
    ensureCsvWithHeader(INVOICE_FILE, "id,customerId,rentalId,driverId,subtotal,discountAmount,taxAmount,totalAmount,status,paymentMethod,paymentReference,promoCode,createdAt\n");
    FILE *f = fopen(INVOICE_FILE, "r");
    if (!f)
    {
        *head = NULL;
        return;
    }
    char line[512];
    fgets(line, sizeof(line), f);
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        Invoice *inv = parseInvoiceCSV(line);
        if (inv)
        {
            inv->next = *head;
            *head = inv;
        }
    }
    fclose(f);
}

void saveInvoices(Invoice *head)
{
    FILE *f = fopen(INVOICE_FILE, "w");
    if (!f)
    {
        printf("Error: could not open %s\n", INVOICE_FILE);
        return;
    }
    fprintf(f, "id,customerId,rentalId,driverId,subtotal,discountAmount,taxAmount,totalAmount,status,paymentMethod,paymentReference,promoCode,createdAt\n");
    for (Invoice *inv = head; inv; inv = inv->next)
    {
        fprintf(f, "%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%d,%d,%s,%s,%ld\n",
                inv->id, inv->customerId, inv->rentalId, inv->driverId,
                inv->subtotal, inv->discountAmount, inv->taxAmount, inv->totalAmount,
                (int)inv->status, (int)inv->paymentMethod, inv->paymentReference, inv->promoCode, (long)inv->createdAt);
    }
    fclose(f);
}

void displayInvoice(const Invoice *invoice)
{
    if (!invoice)
        return;

    printf("\n--- Invoice Details ---\n");
    printf("ID: %d\n", invoice->id);
    printf("Customer ID: %d\n", invoice->customerId);
    printf("Rental ID: %d\n", invoice->rentalId);
    printf("Driver ID: %d\n", invoice->driverId);
    printf("Subtotal: $%.2f\n", invoice->subtotal);
    printf("Discount: $%.2f\n", invoice->discountAmount);
    printf("Tax: $%.2f\n", invoice->taxAmount);
    printf("Total: $%.2f\n", invoice->totalAmount);
    printf("Status: %s\n", invoiceStatusStr(invoice->status));
    printf("Payment Method: %s\n", paymentMethodStr(invoice->paymentMethod));
    if (invoice->promoCode[0])
        printf("Promo Code: %s\n", invoice->promoCode);
    if (invoice->paymentReference[0])
        printf("Payment Reference: %s\n", invoice->paymentReference);
    printf("Created: %s", ctime(&invoice->createdAt));
}

void listAllInvoices(Invoice *head)
{
    if (!head)
    {
        printf("No invoices found.\n");
        return;
    }

    printf("\n--- All Invoices ---\n");
    printf("%-8s %-8s %-8s %-8s %-10s %-10s %-8s\n", "ID", "Customer", "Rental", "Driver", "Subtotal", "Total", "Status");
    printf("------------------------------------------------------------\n");

    for (Invoice *inv = head; inv; inv = inv->next)
    {
        printf("%-8d %-8d %-8d %-8d $%-9.2f $%-9.2f %-8s\n",
               inv->id, inv->customerId, inv->rentalId, inv->driverId,
               inv->subtotal, inv->totalAmount, invoiceStatusStr(inv->status));
    }
}

void listInvoicesByCustomer(Invoice *head, int customerId)
{
    if (!head)
    {
        printf("No invoices found.\n");
        return;
    }

    printf("\n--- Invoices for Customer %d ---\n", customerId);
    printf("%-8s %-8s %-8s %-10s %-10s %-8s\n", "ID", "Rental", "Driver", "Subtotal", "Total", "Status");
    printf("------------------------------------------------\n");

    int found = 0;
    for (Invoice *inv = head; inv; inv = inv->next)
    {
        if (inv->customerId == customerId)
        {
            printf("%-8d %-8d %-8d $%-9.2f $%-9.2f %-8s\n",
                   inv->id, inv->rentalId, inv->driverId,
                   inv->subtotal, inv->totalAmount, invoiceStatusStr(inv->status));
            found = 1;
        }
    }

    if (!found)
        printf("No invoices found for customer %d.\n", customerId);
}

void listInvoicesByStatus(Invoice *head, InvoiceStatus status)
{
    if (!head)
    {
        printf("No invoices found.\n");
        return;
    }

    printf("\n--- Invoices with Status: %s ---\n", invoiceStatusStr(status));
    printf("%-8s %-8s %-8s %-8s %-10s %-10s\n", "ID", "Customer", "Rental", "Driver", "Subtotal", "Total");
    printf("------------------------------------------------\n");

    int found = 0;
    for (Invoice *inv = head; inv; inv = inv->next)
    {
        if (inv->status == status)
        {
            printf("%-8d %-8d %-8d %-8d $%-9.2f $%-9.2f\n",
                   inv->id, inv->customerId, inv->rentalId, inv->driverId,
                   inv->subtotal, inv->totalAmount);
            found = 1;
        }
    }

    if (!found)
        printf("No invoices found with status %s.\n", invoiceStatusStr(status));
}

int processPayment(Invoice *invoice, PaymentMethod method, const char *paymentRef)
{
    if (!invoice)
        return 0;

    if (invoice->status == INVOICE_PAID)
    {
        printf("Invoice is already paid.\n");
        return 0;
    }

    updateInvoiceStatus(invoice, INVOICE_PAID, method, paymentRef);
    printf("Payment processed successfully!\n");
    return 1;
}

void refundInvoice(Invoice *invoice, const char *reason)
{
    if (!invoice)
        return;

    if (invoice->status != INVOICE_PAID)
    {
        printf("Invoice is not paid, cannot refund.\n");
        return;
    }

    updateInvoiceStatus(invoice, INVOICE_REFUNDED, PAYMENT_CASH, "");
    printf("Invoice refunded successfully. Reason: %s\n", reason ? reason : "No reason provided");
}

void showInvoiceStatistics(Invoice *head)
{
    if (!head)
    {
        printf("No invoices found.\n");
        return;
    }

    int total = 0, pending = 0, paid = 0, cancelled = 0, refunded = 0;
    float totalRevenue = 0.0, totalDiscounts = 0.0;

    for (Invoice *inv = head; inv; inv = inv->next)
    {
        total++;
        totalRevenue += inv->totalAmount;
        totalDiscounts += inv->discountAmount;

        switch (inv->status)
        {
        case INVOICE_PENDING:
            pending++;
            break;
        case INVOICE_PAID:
            paid++;
            break;
        case INVOICE_CANCELLED:
            cancelled++;
            break;
        case INVOICE_REFUNDED:
            refunded++;
            break;
        }
    }

    printf("\n--- Invoice Statistics ---\n");
    printf("Total Invoices: %d\n", total);
    printf("Pending: %d | Paid: %d | Cancelled: %d | Refunded: %d\n", pending, paid, cancelled, refunded);
    printf("Total Revenue: $%.2f\n", totalRevenue);
    printf("Total Discounts Given: $%.2f\n", totalDiscounts);
    printf("Average Invoice Value: $%.2f\n", total > 0 ? totalRevenue / total : 0.0);
}

void showPaymentMethodStats(Invoice *head)
{
    if (!head)
    {
        printf("No invoices found.\n");
        return;
    }

    int cash = 0, card = 0, mobile = 0;
    float cashTotal = 0.0, cardTotal = 0.0, mobileTotal = 0.0;

    for (Invoice *inv = head; inv; inv = inv->next)
    {
        if (inv->status == INVOICE_PAID)
        {
            switch (inv->paymentMethod)
            {
            case PAYMENT_CASH:
                cash++;
                cashTotal += inv->totalAmount;
                break;
            case PAYMENT_CARD:
                card++;
                cardTotal += inv->totalAmount;
                break;
            case PAYMENT_MOBILE_BANKING:
                mobile++;
                mobileTotal += inv->totalAmount;
                break;
            case PAYMENT_CRYPTO:
                mobile++;
                mobileTotal += inv->totalAmount;
                break;
            }
        }
    }

    printf("\n--- Payment Method Statistics ---\n");
    printf("Cash Payments: %d (Total: $%.2f)\n", cash, cashTotal);
    printf("Card Payments: %d (Total: $%.2f)\n", card, cardTotal);
    printf("Mobile Payments: %d (Total: $%.2f)\n", mobile, mobileTotal);
}

void freeInvoiceList(Invoice **head)
{
    Invoice *current = *head;
    Invoice *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}

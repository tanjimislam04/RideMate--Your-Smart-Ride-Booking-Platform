#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "vehicle.h"

/* ===== Files ===== */
#define VEHICLE_FILE "vehicles.csv"
#define ROUTE_FILE "routes.csv"

/* ===== Globals ===== */
Route *routeHead = NULL;

/* ===== ID counters ===== */
static int nextVehicleId = 1001;
static int nextRouteId = 5001;

/* ===== Internal helpers ===== */
static int isValidFloatStr(const char *s)
{
    if (!s || !*s)
        return 0;
    int dot = 0;
    const char *p = s;
    if (*p == '+' || *p == '-')
        p++;
    for (; *p; ++p)
    {
        if (*p == '.')
        {
            if (dot)
                return 0;
            dot = 1;
        }
        else if (*p < '0' || *p > '9')
        {
            return 0;
        }
    }
    return 1;
}

const char *vehicleTypeStr(VehicleType t)
{
    switch (t)
    {
    case VTYPE_CAR:
        return "CAR";
    case VTYPE_MOTORCYCLE:
        return "MOTORCYCLE";
    case VTYPE_TRUCK:
        return "TRUCK";
    case VTYPE_VAN:
        return "VAN";
    default:
        return "UNKNOWN";
    }
}

static VehicleType typeFromChoice(int c)
{
    switch (c)
    {
    case 1:
        return VTYPE_CAR;
    case 2:
        return VTYPE_MOTORCYCLE;
    case 3:
        return VTYPE_TRUCK;
    case 4:
        return VTYPE_VAN;
    default:
        return VTYPE_CAR;
    }
}

static void adjustNextVehicleId(Vehicle *head)
{
    int maxId = nextVehicleId - 1;
    for (Vehicle *v = head; v; v = v->next)
        if (v->id > maxId)
            maxId = v->id;
    nextVehicleId = maxId + 1;
}

static void adjustNextRouteId(Route *head)
{
    int maxId = nextRouteId - 1;
    for (Route *r = head; r; r = r->next)
        if (r->id > maxId)
            maxId = r->id;
    nextRouteId = maxId + 1;
}

/* ===== CSV: Vehicles ===== */
/* CSV header: id,make,model,year,type,ratePerHour,ratePerDay,available,active,licensePlate */

static Vehicle *parseVehicleCSV(char *line)
{
    Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
    if (!v)
        return NULL;
    v->next = NULL;
    int id, year, type, available, active;
    float rph, rpd;
    char make[50] = {0}, model[50] = {0}, plate[20] = {0};

    int n = sscanf(line, "%d,%49[^,],%49[^,],%d,%d,%f,%f,%d,%d,%19[^,\n]",
                   &id, make, model, &year, &type, &rph, &rpd, &available, &active, plate);
    if (n != 10)
    {
        free(v);
        return NULL;
    }

    v->id = id;
    strncpy(v->make, make, sizeof(v->make) - 1);
    v->make[sizeof(v->make) - 1] = 0;
    strncpy(v->model, model, sizeof(v->model) - 1);
    v->model[sizeof(v->model) - 1] = 0;
    v->year = year;
    v->type = (VehicleType)type;
    v->ratePerHour = rph;
    v->ratePerDay = rpd;
    v->available = available;
    v->active = active;
    strncpy(v->licensePlate, plate, sizeof(v->licensePlate) - 1);
    v->licensePlate[sizeof(v->licensePlate) - 1] = 0;

    return v;
}

void loadVehicles(Vehicle **head)
{
    *head = NULL;
    FILE *f = fopen(VEHICLE_FILE, "r");
    if (!f)
        return;

    char line[512];
    /* skip header if present */
    if (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "id,", 3) != 0)
        {
            fseek(f, 0, SEEK_SET);
        }
    }

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\r\n")] = 0;
        if (!line[0])
            continue;
        Vehicle *v = parseVehicleCSV(line);
        if (v)
        {
            v->next = *head;
            *head = v;
        }
    }
    fclose(f);
    adjustNextVehicleId(*head);
}

void saveVehicles(Vehicle *head)
{
    FILE *f = fopen(VEHICLE_FILE, "w");
    if (!f)
    {
        printf("Error opening %s\n", VEHICLE_FILE);
        return;
    }
    fprintf(f, "id,make,model,year,type,ratePerHour,ratePerDay,available,active,licensePlate\n");
    for (Vehicle *v = head; v; v = v->next)
    {
        fprintf(f, "%d,%s,%s,%d,%d,%.2f,%.2f,%d,%d,%s\n",
                v->id, v->make, v->model, v->year, (int)v->type,
                v->ratePerHour, v->ratePerDay, v->available, v->active, v->licensePlate);
    }
    fclose(f);
}

/* ===== CSV: Routes ===== */
/* CSV header: id,start,end,baseFare,etaMin */

static Route *parseRouteCSV(char *line)
{
    Route *r = (Route *)malloc(sizeof(Route));
    if (!r)
        return NULL;
    r->next = NULL;

    int id, eta;
    float fare;
    char s[50] = {0}, e[50] = {0};

    int n = sscanf(line, "%d,%49[^,],%49[^,],%f,%d",
                   &id, s, e, &fare, &eta);
    if (n != 5)
    {
        free(r);
        return NULL;
    }

    r->id = id;
    strncpy(r->start, s, sizeof(r->start) - 1);
    r->start[sizeof(r->start) - 1] = 0;
    strncpy(r->end, e, sizeof(r->end) - 1);
    r->end[sizeof(r->end) - 1] = 0;
    r->baseFare = fare;
    r->etaMin = eta;

    return r;
}

void loadRoutes(Route **head)
{
    *head = NULL;
    FILE *f = fopen(ROUTE_FILE, "r");
    if (!f)
        return;

    char line[512];
    /* skip header if present */
    if (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "id,", 3) != 0)
        {
            fseek(f, 0, SEEK_SET);
        }
    }

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\r\n")] = 0;
        if (!line[0])
            continue;
        Route *r = parseRouteCSV(line);
        if (r)
        {
            r->next = *head;
            *head = r;
        }
    }
    fclose(f);
    adjustNextRouteId(*head);
    routeHead = *head; /* make global visible */
}

void saveRoutes(Route *head)
{
    FILE *f = fopen(ROUTE_FILE, "w");
    if (!f)
    {
        printf("Error opening %s\n", ROUTE_FILE);
        return;
    }
    fprintf(f, "id,start,end,baseFare,etaMin\n");
    for (Route *r = head; r; r = r->next)
    {
        fprintf(f, "%d,%s,%s,%.2f,%d\n", r->id, r->start, r->end, r->baseFare, r->etaMin);
    }
    fclose(f);
    routeHead = head; /* keep global consistent */
}

/* ===== Lookups / Displays ===== */

Vehicle *findVehicleById(Vehicle *head, int id)
{
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->id == id)
            return v;
    }
    return NULL;
}

void displayAvailableVehicles(Vehicle *head)
{
    int found = 0;
    printf("\n--- Available Vehicles ---\n");
    printf("%-5s %-10s %-12s %-6s %-10s %-10s %-10s %-10s\n",
           "ID", "Make", "Model", "Year", "Type", "Rate/hr", "Rate/day", "Plate");
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->active && v->available)
        {
            printf("%-5d %-10s %-12s %-6d %-10s %-10.2f %-10.2f %-10s\n",
                   v->id, v->make, v->model, v->year, vehicleTypeStr(v->type),
                   v->ratePerHour, v->ratePerDay, v->licensePlate);
            found = 1;
        }
    }
    if (!found)
        printf("None.\n");
}

void displayAllVehicles(Vehicle *head)
{
    printf("\n--- All Vehicles ---\n");
    printf("%-5s %-10s %-12s %-6s %-10s %-10s %-10s %-6s %-6s %-10s\n",
           "ID", "Make", "Model", "Year", "Type", "Rate/hr", "Rate/day", "Avail", "Active", "Plate");
    for (Vehicle *v = head; v; v = v->next)
    {
        printf("%-5d %-10s %-12s %-6d %-10s %-10.2f %-10.2f %-6s %-6s %-10s\n",
               v->id, v->make, v->model, v->year, vehicleTypeStr(v->type),
               v->ratePerHour, v->ratePerDay,
               v->available ? "Yes" : "No",
               v->active ? "Yes" : "No",
               v->licensePlate);
    }
}

void displayAllRoutes(Route *rhead)
{
    printf("\n--- Routes ---\n");
    printf("%-5s %-18s %-18s %-10s %-8s\n", "ID", "Start", "End", "BaseFare", "ETA(min)");
    for (Route *r = rhead; r; r = r->next)
    {
        printf("%-5d %-18s %-18s %-10.2f %-8d\n",
               r->id, r->start, r->end, r->baseFare, r->etaMin);
    }
}

Route *findRouteById(Route *rhead, int routeId)
{
    for (Route *r = rhead; r; r = r->next)
        if (r->id == routeId)
            return r;
    return NULL;
}

/* ===== Admin helpers ===== */

static void addVehiclePrompt(Vehicle **head)
{
    Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
    if (!v)
    {
        printf("Memory error.\n");
        return;
    }
    memset(v, 0, sizeof(*v));
    v->id = nextVehicleId++;
    v->available = 1;
    v->active = 1;

    printf("\n--- Add Vehicle ---\n");
    getInput("Make: ", v->make, sizeof(v->make));
    getInput("Model: ", v->model, sizeof(v->model));

    char buf[64];
    while (1)
    {
        getInput("Year (e.g., 2019): ", buf, sizeof(buf));
        if (isValidNumber(buf))
        {
            v->year = atoi(buf);
            break;
        }
        printf("Invalid year.\n");
    }

    printf("Type: 1) CAR  2) MOTORCYCLE  3) TRUCK  4) VAN\n");
    while (1)
    {
        getInput("Choose type (1-4): ", buf, sizeof(buf));
        if (isValidNumber(buf))
        {
            int c = atoi(buf);
            if (c >= 1 && c <= 4)
            {
                v->type = typeFromChoice(c);
                break;
            }
        }
        printf("Invalid choice.\n");
    }

    while (1)
    {
        getInput("Rate per hour: ", buf, sizeof(buf));
        if (isValidFloatStr(buf))
        {
            v->ratePerHour = (float)atof(buf);
            break;
        }
        printf("Invalid float value.\n");
    }
    while (1)
    {
        getInput("Rate per day: ", buf, sizeof(buf));
        if (isValidFloatStr(buf))
        {
            v->ratePerDay = (float)atof(buf);
            break;
        }
        printf("Invalid float value.\n");
    }
    getInput("License plate: ", v->licensePlate, sizeof(v->licensePlate));

    v->next = *head;
    *head = v;
    saveVehicles(*head);
    printf("Vehicle added with ID %d\n", v->id);
}

static void updateVehiclePrompt(Vehicle *head)
{
    char buf[64];
    getInput("Enter Vehicle ID to update: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid ID.\n");
        return;
    }
    int id = atoi(buf);

    Vehicle *v = findVehicleById(head, id);
    if (!v)
    {
        printf("Vehicle not found.\n");
        return;
    }

    printf("\n--- Update Vehicle (leave blank to keep old) ---\n");

    char tmp[64];
    printf("Make [%s]: ", v->make);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp))
        strncpy(v->make, tmp, sizeof(v->make) - 1);

    printf("Model [%s]: ", v->model);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp))
        strncpy(v->model, tmp, sizeof(v->model) - 1);

    printf("Year [%d]: ", v->year);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp) && isValidNumber(tmp))
        v->year = atoi(tmp);

    printf("Type [%s] (1 CAR,2 MOTO,3 TRUCK,4 VAN): ", vehicleTypeStr(v->type));
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp) && isValidNumber(tmp))
    {
        int c = atoi(tmp);
        if (c >= 1 && c <= 4)
            v->type = typeFromChoice(c);
    }

    printf("Rate per hour [%.2f]: ", v->ratePerHour);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp) && isValidFloatStr(tmp))
        v->ratePerHour = (float)atof(tmp);

    printf("Rate per day [%.2f]: ", v->ratePerDay);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp) && isValidFloatStr(tmp))
        v->ratePerDay = (float)atof(tmp);

    printf("Available [%s] (1/0): ", v->available ? "1" : "0");
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp) && (tmp[0] == '0' || tmp[0] == '1'))
        v->available = (tmp[0] == '1');

    printf("Active [%s] (1/0): ", v->active ? "1" : "0");
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp) && (tmp[0] == '0' || tmp[0] == '1'))
        v->active = (tmp[0] == '1');

    printf("License plate [%s]: ", v->licensePlate);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp))
        strncpy(v->licensePlate, tmp, sizeof(v->licensePlate) - 1);

    saveVehicles(head);
    printf("Vehicle updated.\n");
}

static void softDeleteVehiclePrompt(Vehicle *head)
{
    char buf[32];
    getInput("Enter Vehicle ID to soft delete: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid ID.\n");
        return;
    }
    int id = atoi(buf);

    Vehicle *v = findVehicleById(head, id);
    if (!v)
    {
        printf("Vehicle not found.\n");
        return;
    }
    v->active = 0;
    v->available = 0;
    saveVehicles(head);
    printf("Vehicle #%d marked inactive.\n", id);
}

static void searchFilterVehicles(Vehicle *head)
{
    char buf[32];
    printf("\nSearch/Filter:\n");
    printf("1) By Type\n2) By Availability\n3) By Price Range (Rate/day)\n4) Back\n");
    getInput("Choose: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
        return;
    int ch = atoi(buf);

    if (ch == 1)
    {
        printf("Type: 1 CAR, 2 MOTORCYCLE, 3 TRUCK, 4 VAN\n");
        getInput("Choose type: ", buf, sizeof(buf));
        if (!isValidNumber(buf))
            return;
        int t = atoi(buf);
        VehicleType vt = typeFromChoice(t);

        printf("\n-- Vehicles of type %s --\n", vehicleTypeStr(vt));
        printf("%-5s %-10s %-12s %-6s %-10s %-10s %-10s %-6s %-6s\n",
               "ID", "Make", "Model", "Year", "Type", "Rate/hr", "Rate/day", "Avail", "Active");
        for (Vehicle *v = head; v; v = v->next)
        {
            if (v->active && v->type == vt)
            {
                printf("%-5d %-10s %-12s %-6d %-10s %-10.2f %-10.2f %-6s %-6s\n",
                       v->id, v->make, v->model, v->year, vehicleTypeStr(v->type),
                       v->ratePerHour, v->ratePerDay,
                       v->available ? "Yes" : "No", v->active ? "Yes" : "No");
            }
        }
    }
    else if (ch == 2)
    {
        printf("\n-- Vehicles by availability (Active only) --\n");
        printf("%-5s %-10s %-12s %-6s %-10s %-10s %-10s %-6s\n",
               "ID", "Make", "Model", "Year", "Type", "Rate/hr", "Rate/day", "Avail");
        for (Vehicle *v = head; v; v = v->next)
        {
            if (v->active)
            {
                printf("%-5d %-10s %-12s %-6d %-10s %-10.2f %-10.2f %-6s\n",
                       v->id, v->make, v->model, v->year, vehicleTypeStr(v->type),
                       v->ratePerHour, v->ratePerDay, v->available ? "Yes" : "No");
            }
        }
    }
    else if (ch == 3)
    {
        char lo[32], hi[32];
        getInput("Min rate/day: ", lo, sizeof(lo));
        getInput("Max rate/day: ", hi, sizeof(hi));
        if (!isValidFloatStr(lo) || !isValidFloatStr(hi))
        {
            printf("Invalid range.\n");
            return;
        }
        float vlo = (float)atof(lo), vhi = (float)atof(hi);
        if (vlo > vhi)
        {
            float t = vlo;
            vlo = vhi;
            vhi = t;
        }

        printf("\n-- Vehicles with rate/day in [%.2f, %.2f] --\n", vlo, vhi);
        printf("%-5s %-10s %-12s %-6s %-10s %-10s %-10s %-6s\n",
               "ID", "Make", "Model", "Year", "Type", "Rate/hr", "Rate/day", "Avail");
        for (Vehicle *v = head; v; v = v->next)
        {
            if (v->active && v->ratePerDay >= vlo && v->ratePerDay <= vhi)
            {
                printf("%-5d %-10s %-12s %-6d %-10s %-10.2f %-10.2f %-6s\n",
                       v->id, v->make, v->model, v->year, vehicleTypeStr(v->type),
                       v->ratePerHour, v->ratePerDay, v->available ? "Yes" : "No");
            }
        }
    }
}

/* ===== Routes admin ===== */

static void addRoutePrompt(Route **rhead)
{
    Route *r = (Route *)malloc(sizeof(Route));
    if (!r)
    {
        printf("Memory error.\n");
        return;
    }
    memset(r, 0, sizeof(*r));
    r->id = nextRouteId++;

    printf("\n--- Add Route ---\n");
    getInput("Start location: ", r->start, sizeof(r->start));
    getInput("End location: ", r->end, sizeof(r->end));

    char buf[32];
    while (1)
    {
        getInput("Base fare: ", buf, sizeof(buf));
        if (isValidFloatStr(buf))
        {
            r->baseFare = (float)atof(buf);
            break;
        }
        printf("Invalid number.\n");
    }
    while (1)
    {
        getInput("ETA (minutes): ", buf, sizeof(buf));
        if (isValidNumber(buf))
        {
            r->etaMin = atoi(buf);
            break;
        }
        printf("Invalid minutes.\n");
    }

    r->next = *rhead;
    *rhead = r;
    saveRoutes(*rhead);
    printf("Route added with ID %d\n", r->id);
    routeHead = *rhead;
}

static void updateRoutePrompt(Route *rhead)
{
    char buf[32];
    getInput("Enter Route ID to update: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid ID.\n");
        return;
    }
    int id = atoi(buf);

    Route *r = findRouteById(rhead, id);
    if (!r)
    {
        printf("Route not found.\n");
        return;
    }

    char tmp[64];
    printf("Start [%s]: ", r->start);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp))
        strncpy(r->start, tmp, sizeof(r->start) - 1);

    printf("End [%s]: ", r->end);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp))
        strncpy(r->end, tmp, sizeof(r->end) - 1);

    printf("Base fare [%.2f]: ", r->baseFare);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp) && isValidFloatStr(tmp))
        r->baseFare = (float)atof(tmp);

    printf("ETA (min) [%d]: ", r->etaMin);
    getInput("", tmp, sizeof(tmp));
    if (strlen(tmp) && isValidNumber(tmp))
        r->etaMin = atoi(tmp);

    saveRoutes(rhead);
    printf("Route updated.\n");
    routeHead = rhead;
}

static void deleteRoutePrompt(Route **rhead)
{
    char buf[32];
    getInput("Enter Route ID to delete: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid ID.\n");
        return;
    }
    int id = atoi(buf);

    Route *prev = NULL, *cur = *rhead;
    while (cur)
    {
        if (cur->id == id)
        {
            if (prev)
                prev->next = cur->next;
            else
                *rhead = cur->next;
            free(cur);
            saveRoutes(*rhead);
            printf("Route deleted.\n");
            routeHead = *rhead;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
    printf("Route not found.\n");
}

void adminRoutesMenu(Route **rhead)
{
    char input[16];
    int opt;
    while (1)
    {
        printf("\n=== Admin Routes Menu ===\n");
        printf("1. Add Route\n");
        printf("2. List Routes\n");
        printf("3. Update Route\n");
        printf("4. Delete Route\n");
        printf("5. Back\n");
        getInput("Choose: ", input, sizeof(input));
        if (!isValidNumber(input))
        {
            printf("Invalid.\n");
            continue;
        }
        opt = atoi(input);

        switch (opt)
        {
        case 1:
            addRoutePrompt(rhead);
            break;
        case 2:
            displayAllRoutes(*rhead);
            break;
        case 3:
            updateRoutePrompt(*rhead);
            saveRoutes(*rhead);
            break;
        case 4:
            deleteRoutePrompt(rhead);
            break;
        case 5:
            return;
        default:
            printf("Invalid.\n");
        }
    }
}

/* ===== Admin Vehicle Menu ===== */

void adminVehicleMenu(Vehicle **head)
{
    /* Ensure routes are loaded for route-aware rentals */
    if (!routeHead)
    {
        loadRoutes(&routeHead);
    }

    char input[16];
    int choice;
    while (1)
    {
        printf("\n=== Admin Vehicle Menu ===\n");
        printf("1. Add Vehicle\n");
        printf("2. List All Vehicles\n");
        printf("3. Search / Filter Vehicles\n");
        printf("4. Update Vehicle\n");
        printf("5. Soft Delete Vehicle\n");
        printf("6. Manage Routes\n");
        printf("7. Back\n");
        getInput("Choose: ", input, sizeof(input));
        if (!isValidNumber(input))
        {
            printf("Invalid.\n");
            continue;
        }
        choice = atoi(input);

        switch (choice)
        {
        case 1:
            addVehiclePrompt(head);
            break;
        case 2:
            displayAllVehicles(*head);
            break;
        case 3:
            searchFilterVehicles(*head);
            break;
        case 4:
            updateVehiclePrompt(*head);
            break;
        case 5:
            softDeleteVehiclePrompt(*head);
            break;
        case 6:
            adminRoutesMenu(&routeHead);
            break;
        case 7:
            return;
        default:
            printf("Invalid choice.\n");
        }
    }
}

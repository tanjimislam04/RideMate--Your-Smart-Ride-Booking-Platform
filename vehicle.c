// vehicle.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "vehicle.h"

/* ---------- Files ---------- */
#define VEHICLE_FILE "vehicles.csv"
#define ROUTE_FILE "routes.csv"

/* ---------- Globals (declared extern in vehicle.h) ---------- */
Route *routeHead = NULL; /* rental.c expects this to exist */

/* ---------- ID counters ---------- */
static int nextVehicleId = 2001;
static int nextRouteId = 3001;

/* ---------- Small helpers ---------- */
static void ensureCsvWithHeader(const char *path, const char *headerLine)
{
    FILE *r = fopen(path, "r");
    if (r)
    {
        fclose(r);
        return;
    } /* already exists */
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

static void adjustVehicleNextId(Vehicle *head)
{
    int maxId = nextVehicleId - 1;
    for (Vehicle *v = head; v; v = v->next)
        if (v->id > maxId)
            maxId = v->id;
    nextVehicleId = maxId + 1;
}

static void adjustRouteNextId(Route *head)
{
    int maxId = nextRouteId - 1;
    for (Route *r = head; r; r = r->next)
        if (r->id > maxId)
            maxId = r->id;
    nextRouteId = maxId + 1;
}

static const char *typeName(VehicleType t)
{
    switch (t)
    {
    case VEH_CAR:
        return "CAR";
    case VEH_MOTORCYCLE:
        return "MOTORCYCLE";
    case VEH_TRUCK:
        return "TRUCK";
    case VEH_VAN:
        return "VAN";
    default:
        return "UNKNOWN";
    }
}

/* ---------- CSV parse ---------- */
static Vehicle *parseVehicleCSV(char *line)
{
    /* CSV: id,make,model,year,type,ratePerDay,ratePerHour,active,available */
    Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
    if (!v)
        return NULL;
    memset(v, 0, sizeof(*v));

    int id, year, type, active, avail;
    float rpd, rph;
    char make[50] = {0}, model[50] = {0};

    int n = sscanf(line, "%d,%49[^,],%49[^,],%d,%d,%f,%f,%d,%d",
                   &id, make, model, &year, &type, &rpd, &rph, &active, &avail);
    if (n != 9)
    {
        free(v);
        return NULL;
    }

    v->id = id;
    strncpy(v->make, make, sizeof(v->make) - 1);
    v->make[sizeof(v->make) - 1] = '\0';
    strncpy(v->model, model, sizeof(v->model) - 1);
    v->model[sizeof(v->model) - 1] = '\0';
    v->year = year;
    v->type = (VehicleType)type;
    v->ratePerDay = rpd;
    v->ratePerHour = rph;
    v->active = active ? 1 : 0;
    v->available = avail ? 1 : 0;
    v->next = NULL;
    return v;
}

static Route *parseRouteCSV(char *line)
{
    /* CSV: id,name,from,to,baseFare,etaMin,active */
    Route *r = (Route *)malloc(sizeof(Route));
    if (!r)
        return NULL;
    memset(r, 0, sizeof(*r));

    int id, eta, active;
    float fare;
    char name[64] = {0}, from[64] = {0}, to[64] = {0};

    int n = sscanf(line, "%d,%63[^,],%63[^,],%63[^,],%f,%d,%d",
                   &id, name, from, to, &fare, &eta, &active);
    if (n != 7)
    {
        free(r);
        return NULL;
    }

    r->id = id;
    strncpy(r->name, name, sizeof(r->name) - 1);
    r->name[sizeof(r->name) - 1] = '\0';
    strncpy(r->from, from, sizeof(r->from) - 1);
    r->from[sizeof(r->from) - 1] = '\0';
    strncpy(r->to, to, sizeof(r->to) - 1);
    r->to[sizeof(r->to) - 1] = '\0';
    r->baseFare = fare;
    r->etaMin = eta;
    r->active = active ? 1 : 0;
    r->next = NULL;
    return r;
}

/* ---------- Load / Save ---------- */
void loadVehicles(Vehicle **headRef)
{
    ensureCsvWithHeader(VEHICLE_FILE,
                        "id,make,model,year,type,ratePerDay,ratePerHour,active,available");

    *headRef = NULL;

    FILE *f = fopen(VEHICLE_FILE, "r");
    if (!f)
        return;

    char line[512];
    /* Skip header if first line is header */
    if (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "id,", 3) != 0)
        {
            fseek(f, 0, SEEK_SET);
        }
    }

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        if (!line[0])
            continue;
        Vehicle *v = parseVehicleCSV(line);
        if (v)
        {
            v->next = *headRef;
            *headRef = v;
        }
    }
    fclose(f);

    adjustVehicleNextId(*headRef);
}

void saveVehicles(Vehicle *head)
{
    FILE *f = fopen(VEHICLE_FILE, "w");
    if (!f)
    {
        printf("Error: cannot write %s\n", VEHICLE_FILE);
        return;
    }

    fprintf(f, "id,make,model,year,type,ratePerDay,ratePerHour,active,available\n");
    for (Vehicle *v = head; v; v = v->next)
    {
        fprintf(f, "%d,%s,%s,%d,%d,%.2f,%.2f,%d,%d\n",
                v->id, v->make, v->model, v->year, (int)v->type,
                v->ratePerDay, v->ratePerHour, v->active, v->available);
    }
    fclose(f);
}

void loadRoutes(Route **headRef)
{
    ensureCsvWithHeader(ROUTE_FILE,
                        "id,name,from,to,baseFare,etaMin,active");

    *headRef = NULL;

    FILE *f = fopen(ROUTE_FILE, "r");
    if (!f)
        return;

    char line[512];
    if (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "id,", 3) != 0)
        {
            fseek(f, 0, SEEK_SET);
        }
    }

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        if (!line[0])
            continue;
        Route *r = parseRouteCSV(line);
        if (r)
        {
            r->next = *headRef;
            *headRef = r;
        }
    }
    fclose(f);

    adjustRouteNextId(*headRef);
}

void saveRoutes(Route *head)
{
    FILE *f = fopen(ROUTE_FILE, "w");
    if (!f)
    {
        printf("Error: cannot write %s\n", ROUTE_FILE);
        return;
    }

    fprintf(f, "id,name,from,to,baseFare,etaMin,active\n");
    for (Route *r = head; r; r = r->next)
    {
        fprintf(f, "%d,%s,%s,%s,%.2f,%d,%d\n",
                r->id, r->name, r->from, r->to, r->baseFare, r->etaMin, r->active);
    }
    fclose(f);
}

/* ---------- Find ---------- */
Vehicle *findVehicleById(Vehicle *head, int id)
{
    for (Vehicle *v = head; v; v = v->next)
        if (v->id == id)
            return v;
    return NULL;
}

Route *findRouteById(Route *head, int id)
{
    for (Route *r = head; r; r = r->next)
        if (r->id == id)
            return r;
    return NULL;
}

/* ---------- Display ---------- */
void displayAvailableVehicles(Vehicle *head)
{
    int any = 0;
    printf("\n--- Available Vehicles ---\n");
    printf("%-5s %-10s %-12s %-6s %-12s %-10s %-10s\n",
           "ID", "Make", "Model", "Year", "Type", "Day($)", "Hour($)");
    for (Vehicle *v = head; v; v = v->next)
    {
        if (v->active && v->available)
        {
            printf("%-5d %-10s %-12s %-6d %-12s %-10.2f %-10.2f\n",
                   v->id, v->make, v->model, v->year, typeName(v->type),
                   v->ratePerDay, v->ratePerHour);
            any = 1;
        }
    }
    if (!any)
        printf("No vehicles currently available.\n");
}

static void displayAllVehiclesInternal(Vehicle *head)
{
    printf("\n--- All Vehicles ---\n");
    printf("%-5s %-10s %-12s %-6s %-12s %-10s %-10s %-8s %-10s\n",
           "ID", "Make", "Model", "Year", "Type", "Day($)", "Hour($)", "Active", "Available");
    for (Vehicle *v = head; v; v = v->next)
    {
        printf("%-5d %-10s %-12s %-6d %-12s %-10.2f %-10.2f %-8s %-10s\n",
               v->id, v->make, v->model, v->year, typeName(v->type),
               v->ratePerDay, v->ratePerHour,
               v->active ? "Yes" : "No",
               v->available ? "Yes" : "No");
    }
}

void displayAllRoutes(Route *head)
{
    if (!head)
    {
        printf("No routes found.\n");
        return;
    }
    printf("%-5s %-16s %-14s %-14s %-10s %-7s %-7s\n",
           "ID", "Name", "From", "To", "Fare($)", "ETA", "Active");
    for (Route *r = head; r; r = r->next)
    {
        printf("%-5d %-16s %-14s %-14s %-10.2f %-7d %-7s\n",
               r->id, r->name, r->from, r->to, r->baseFare, r->etaMin,
               r->active ? "Yes" : "No");
    }
}

/* ---------- Admin ops ---------- */
static int readVehicleType()
{
    char buf[16];
    printf("Select Type: 0)CAR 1)MOTORCYCLE 2)TRUCK 3)VAN\n");
    getInput("Enter (0-3): ", buf, sizeof(buf));
    if (!isValidNumber(buf))
        return -1;
    int t = atoi(buf);
    if (t < 0 || t > 3)
        return -1;
    return t;
}

static void addVehicleInteractive(Vehicle **headRef)
{
    Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
    if (!v)
    {
        printf("Mem error.\n");
        return;
    }
    memset(v, 0, sizeof(*v));

    v->id = nextVehicleId++;

    getInput("Make: ", v->make, sizeof(v->make));
    getInput("Model: ", v->model, sizeof(v->model));

    char buf[32];
    getInput("Year (e.g., 2018): ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid year.\n");
        free(v);
        return;
    }
    v->year = atoi(buf);

    int t = readVehicleType();
    if (t < 0)
    {
        printf("Invalid type.\n");
        free(v);
        return;
    }
    v->type = (VehicleType)t;

    getInput("Rate per day: ", buf, sizeof(buf));
    if (!strlen(buf))
    {
        printf("Invalid.\n");
        free(v);
        return;
    }
    v->ratePerDay = (float)atof(buf);

    getInput("Rate per hour: ", buf, sizeof(buf));
    if (!strlen(buf))
    {
        printf("Invalid.\n");
        free(v);
        return;
    }
    v->ratePerHour = (float)atof(buf);

    v->active = 1;
    v->available = 1;

    v->next = *headRef;
    *headRef = v;
    saveVehicles(*headRef);

    printf("Vehicle added with ID %d\n", v->id);
}

static void updateVehicleInteractive(Vehicle *head)
{
    char buf[32];
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

    printf("\n--- Update Vehicle #%d (%s %s) ---\n", v->id, v->make, v->model);
    printf("1) Change Rate/Day\n");
    printf("2) Change Rate/Hour\n");
    printf("3) Change Year\n");
    printf("4) Change Type\n");
    printf("5) Toggle Active\n");
    printf("6) Toggle Available\n");
    printf("0) Back\n");

    getInput("Choose: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid.\n");
        return;
    }
    int op = atoi(buf);

    switch (op)
    {
    case 1:
        getInput("New rate/day: ", buf, sizeof(buf));
        v->ratePerDay = (float)atof(buf);
        break;
    case 2:
        getInput("New rate/hour: ", buf, sizeof(buf));
        v->ratePerHour = (float)atof(buf);
        break;
    case 3:
        getInput("New year: ", buf, sizeof(buf));
        if (!isValidNumber(buf))
        {
            printf("Invalid.\n");
            return;
        }
        v->year = atoi(buf);
        break;
    case 4:
    {
        int t = readVehicleType();
        if (t < 0)
        {
            printf("Invalid type.\n");
            return;
        }
        v->type = (VehicleType)t;
        break;
    }
    case 5:
        v->active = !v->active;
        printf("Active => %s\n", v->active ? "Yes" : "No");
        break;
    case 6:
        v->available = !v->available;
        printf("Available => %s\n", v->available ? "Yes" : "No");
        break;
    case 0:
        return;
    default:
        printf("Invalid option.\n");
        return;
    }
    saveVehicles(head);
    printf("Vehicle updated.\n");
}

static void toggleVehicleActive(Vehicle *head)
{
    char buf[32];
    getInput("Enter Vehicle ID to toggle active: ", buf, sizeof(buf));
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
    v->active = !v->active;
    printf("Vehicle #%d active => %s\n", v->id, v->active ? "Yes" : "No");
}

/* ---------- Routes (Admin) ---------- */
static void addRouteInteractive()
{
    Route *r = (Route *)malloc(sizeof(Route));
    if (!r)
    {
        printf("Mem error.\n");
        return;
    }
    memset(r, 0, sizeof(*r));
    r->id = nextRouteId++;

    getInput("Route name: ", r->name, sizeof(r->name));
    getInput("From: ", r->from, sizeof(r->from));
    getInput("To: ", r->to, sizeof(r->to));

    char buf[32];
    getInput("Base Fare: ", buf, sizeof(buf));
    r->baseFare = (float)atof(buf);

    getInput("ETA (minutes): ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid.\n");
        free(r);
        return;
    }
    r->etaMin = atoi(buf);

    r->active = 1;

    r->next = routeHead;
    routeHead = r;
    saveRoutes(routeHead);

    printf("Route added with ID %d\n", r->id);
}

static void updateRouteInteractive()
{
    char buf[32];
    getInput("Enter Route ID to update: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid.\n");
        return;
    }
    int id = atoi(buf);

    Route *r = findRouteById(routeHead, id);
    if (!r)
    {
        printf("Route not found.\n");
        return;
    }

    printf("\n--- Update Route #%d (%s: %s -> %s) ---\n", r->id, r->name, r->from, r->to);
    printf("1) Change name\n");
    printf("2) Change from\n");
    printf("3) Change to\n");
    printf("4) Change base fare\n");
    printf("5) Change ETA (minutes)\n");
    printf("6) Toggle Active\n");
    printf("0) Back\n");

    getInput("Choose: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid.\n");
        return;
    }
    int op = atoi(buf);

    switch (op)
    {
    case 1:
        getInput("New name: ", r->name, sizeof(r->name));
        break;
    case 2:
        getInput("New from: ", r->from, sizeof(r->from));
        break;
    case 3:
        getInput("New to: ", r->to, sizeof(r->to));
        break;
    case 4:
        getInput("New base fare: ", buf, sizeof(buf));
        r->baseFare = (float)atof(buf);
        break;
    case 5:
        getInput("New ETA (min): ", buf, sizeof(buf));
        if (!isValidNumber(buf))
        {
            printf("Invalid.\n");
            return;
        }
        r->etaMin = atoi(buf);
        break;
    case 6:
        r->active = !r->active;
        printf("Active => %s\n", r->active ? "Yes" : "No");
        break;
    case 0:
        return;
    default:
        printf("Invalid option.\n");
        return;
    }
    saveRoutes(routeHead);
    printf("Route updated.\n");
}

static void toggleRouteActive()
{
    char buf[32];
    getInput("Enter Route ID to toggle active: ", buf, sizeof(buf));
    if (!isValidNumber(buf))
    {
        printf("Invalid.\n");
        return;
    }
    int id = atoi(buf);

    Route *r = findRouteById(routeHead, id);
    if (!r)
    {
        printf("Route not found.\n");
        return;
    }
    r->active = !r->active;
    printf("Route #%d active => %s\n", r->id, r->active ? "Yes" : "No");
    saveRoutes(routeHead);
}

static void adminRouteMenu()
{
    char in[16];
    while (1)
    {
        printf("\n=== Route Management ===\n");
        printf("1) List routes\n");
        printf("2) Add route\n");
        printf("3) Update route\n");
        printf("4) Toggle active (soft-delete/restore)\n");
        printf("0) Back\n");
        getInput("Choose: ", in, sizeof(in));
        if (!isValidNumber(in))
        {
            printf("Invalid.\n");
            continue;
        }
        int op = atoi(in);
        switch (op)
        {
        case 1:
            displayAllRoutes(routeHead);
            break;
        case 2:
            addRouteInteractive();
            break;
        case 3:
            updateRouteInteractive();
            break;
        case 4:
            toggleRouteActive();
            break;
        case 0:
            return;
        default:
            printf("Invalid.\n");
            break;
        }
    }
}

/* ---------- Public admin menu ---------- */
void adminVehicleMenu(Vehicle **headRef)
{
    /* Ensure routes are loaded (in case main didn't) */
    if (!routeHead)
        loadRoutes(&routeHead);

    char in[16];
    while (1)
    {
        printf("\n=== Vehicle Management ===\n");
        printf("1) List all vehicles\n");
        printf("2) Add vehicle\n");
        printf("3) Update vehicle\n");
        printf("4) Toggle active (soft-delete/restore)\n");
        printf("5) Manage routes\n");
        printf("0) Back\n");
        getInput("Choose: ", in, sizeof(in));
        if (!isValidNumber(in))
        {
            printf("Invalid.\n");
            continue;
        }
        int op = atoi(in);

        switch (op)
        {
        case 1:
            displayAllVehiclesInternal(*headRef);
            break;
        case 2:
            addVehicleInteractive(headRef);
            break;
        case 3:
            updateVehicleInteractive(*headRef);
            break;
        case 4:
            toggleVehicleActive(*headRef);
            saveVehicles(*headRef);
            break;
        case 5:
            adminRouteMenu();
            break;
        case 0:
            return;
        default:
            printf("Invalid option.\n");
            break;
        }
    }
}

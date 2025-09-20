# RideMate - File Tree Structure

```
RideMate--Your-Smart-Ride-Booking-Platform/
│
├── 📁 Core Application Files
│   ├── main.c                    # Main application entry point
│   ├── RideMate.exe             # Compiled executable
│   └── README.md                # Project documentation
│
├── 📁 Header Files (.h)
│   ├── alert.h                  # Alert system declarations
│   ├── backup.h                 # Backup functionality declarations
│   ├── complaint.h              # Complaint management declarations
│   ├── customer.h               # Customer management declarations
│   ├── dashboard.h              # Dashboard interface declarations
│   ├── driver.h                 # Driver management declarations
│   ├── invoice.h                # Invoice system declarations
│   ├── promo.h                  # Promotional system declarations
│   ├── rating.h                 # Rating system declarations
│   ├── rental.h                 # Rental management declarations
│   ├── reports.h                # Reporting system declarations
│   ├── search.h                 # Search functionality declarations
│   ├── utils.h                  # Utility functions declarations
│   └── vehicle.h                # Vehicle management declarations
│
├── 📁 Source Files (.c)
│   ├── alert.c                  # Alert system implementation
│   ├── backup.c                 # Backup functionality implementation
│   ├── complaint.c              # Complaint management implementation
│   ├── customer.c               # Customer management implementation
│   ├── dashboard.c              # Dashboard interface implementation
│   ├── driver.c                 # Driver management implementation
│   ├── invoice.c                # Invoice system implementation
│   ├── promo.c                  # Promotional system implementation
│   ├── rating.c                 # Rating system implementation
│   ├── rental.c                 # Rental management implementation
│   ├── reports.c                # Reporting system implementation
│   ├── search.c                 # Search functionality implementation
│   ├── utils.c                  # Utility functions implementation
│   └── vehicle.c                # Vehicle management implementation
│
├── 📁 Data Storage
│   ├── 📁 data/                 # Main data directory
│   │   ├── drivers.csv          # Driver information database
│   │   ├── invoices.csv         # Invoice records database
│   │   ├── promos.csv           # Promotional codes database
│   │   ├── routes.csv           # Route information database
│   │   └── vehicles.csv         # Vehicle inventory database
│   │
│   ├── customers.csv            # Customer information database
│   ├── complaints.csv           # Customer complaints database
│   └── rentals.csv              # Rental transaction database
│
├── 📁 System Directories
│   ├── 📁 backups/              # Backup storage (currently empty)
│   ├── 📁 receipts/             # Receipt storage (currently empty)
│   └── 📁 reports/              # Generated reports storage (currently empty)
│
└── 📄 LICENSE                   # Project license file
```

## File Categories

### 🎯 **Core Application**
- `main.c` - Application entry point and main control flow
- `RideMate.exe` - Compiled executable for Windows

### 📋 **Module Structure**
Each feature is implemented as a module with:
- **Header file (.h)** - Function declarations, constants, and data structures
- **Source file (.c)** - Implementation of the module's functionality

### 🗄️ **Data Management**
- **CSV Files** - Store application data in comma-separated format
- **data/** - Centralized data directory for core business entities
- **Root CSV files** - Customer, complaint, and rental data

### 📁 **Storage Directories**
- **backups/** - For system backup files
- **receipts/** - For generated receipt files
- **reports/** - For system-generated reports

## Module Overview

| Module | Purpose |
|--------|---------|
| **alert** | System notifications and alerts |
| **backup** | Data backup and restore functionality |
| **complaint** | Customer complaint management |
| **customer** | Customer account management |
| **dashboard** | Main user interface |
| **driver** | Driver account and management |
| **invoice** | Billing and invoice generation |
| **promo** | Promotional codes and discounts |
| **rating** | Driver and service rating system |
| **rental** | Vehicle rental management |
| **reports** | System reporting and analytics |
| **search** | Search functionality across the system |
| **utils** | Common utility functions |
| **vehicle** | Vehicle inventory management |

## Data Flow

### 🔄 **System Data Flow Architecture**

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   User Input    │───▶│    Dashboard    │───▶│  Module Layer   │
│  (Commands/     │    │   (main.c)      │    │  (Business      │
│   Requests)     │    │                 │    │   Logic)        │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                                        │
                                                        ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Storage        │◀───│  Data Layer     │◀───│  Utils Layer    │
│  Directories    │    │  (CSV Files)    │    │  (utils.c)      │
│  (backups/,     │    │                 │    │                 │
│   receipts/,    │    │ • customers.csv │    │ • Validation    │
│   reports/)     │    │ • drivers.csv   │    │ • File I/O      │
└─────────────────┘    │ • vehicles.csv  │    │ • Calculations  │
                       │ • rentals.csv   │    └─────────────────┘
                       │ • invoices.csv  │
                       │ • complaints.csv│
                       │ • promos.csv    │
                       │ • routes.csv    │
                       └─────────────────┘
```

### 📊 **Detailed Data Flow Process**

#### **1. Input Processing Flow**
```
User Action → Dashboard → Module Selection → Function Execution
     │              │            │                │
     ▼              ▼            ▼                ▼
• Login/Register  • Menu      • customer.c    • Data Validation
• Book Ride      • Routing   • driver.c      • Business Rules
• View History   • Display   • rental.c      • Calculations
• Rate Service   • Input     • invoice.c     • File Operations
```

#### **2. Data Persistence Flow**
```
Module Function → Utils Layer → CSV File → Backup System
      │              │            │           │
      ▼              ▼            ▼           ▼
• Process Data   • File I/O   • Write/Read  • Auto Backup
• Validate      • Error      • Update      • Manual Backup
• Calculate     • Handle     • Delete      • Restore
```

#### **3. Output Generation Flow**
```
Data Retrieval → Report Generation → File Output → User Display
      │                │                │            │
      ▼                ▼                ▼            ▼
• CSV Read       • reports.c      • PDF/Text    • Console
• Data Filter    • Format Data    • receipts/   • Dashboard
• Calculations   • Generate       • reports/    • Alerts
```

### 🔗 **Module Interaction Flow**

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  Customer   │◀──▶│  Dashboard  │◀──▶│   Driver    │
│ Management  │    │             │    │ Management  │
└─────────────┘    └─────────────┘    └─────────────┘
       │                   │                   │
       ▼                   ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Rental    │◀──▶│   Search    │◀──▶│  Vehicle    │
│ Management  │    │             │    │ Management  │
└─────────────┘    └─────────────┘    └─────────────┘
       │                   │                   │
       ▼                   ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Invoice   │◀──▶│   Rating    │◀──▶│   Promo     │
│   System    │    │   System    │    │   System    │
└─────────────┘    └─────────────┘    └─────────────┘
       │                   │                   │
       ▼                   ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  Reports    │◀──▶│   Alert     │◀──▶│   Backup    │
│   System    │    │   System    │    │   System    │
└─────────────┘    └─────────────┘    └─────────────┘
```

### 🎯 **Key Data Flow Patterns**

1. **User Registration/Login**: `customer.c` → `customers.csv` → `dashboard.c`
2. **Ride Booking**: `rental.c` → `rentals.csv` + `vehicles.csv` → `invoice.c`
3. **Driver Assignment**: `driver.c` → `drivers.csv` → `rental.c` → `alert.c`
4. **Payment Processing**: `invoice.c` → `invoices.csv` → `receipts/` → `reports.c`
5. **Rating System**: `rating.c` → `drivers.csv` (update rating) → `reports.c`
6. **Complaint Handling**: `complaint.c` → `complaints.csv` → `alert.c` → `reports.c`
7. **Promo Application**: `promo.c` → `promos.csv` → `invoice.c` (discount calculation)
8. **Backup Operations**: `backup.c` → All CSV files → `backups/` directory

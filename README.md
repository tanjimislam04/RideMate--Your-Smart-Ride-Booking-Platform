## ⭐ User Reviews & Ratings

After completing a rental, customers can rate the vehicle from 1 to 5 stars. These ratings are stored and displayed in the vehicle's details, helping future customers make informed choices and allowing admins to track vehicle performance and satisfaction.

- **How it works:**
  - After rental completion, the customer is prompted to give a star rating (1-5).
  - The average rating and total number of ratings are shown in the vehicle details view.
  - Admins can view ratings analytics in the dashboard and reports.

# RideMate: Your Smart Ride Booking Platform

## Description

RideMate is a comprehensive, console-based Vehicle Rental Management System built in C. It features a modular architecture with advanced data structures (linked lists) for efficient management of vehicles, customers, rentals, drivers, and promotional codes. The system provides both customer and administrative interfaces with persistent data storage, robust error handling, and enhanced security features.

## 🚀 Features & Highlights

### 🆕 Latest Features (2025)

- **🔐 Password Security**: DJB2 hashing algorithm with backward compatibility
- **🛡️ Enhanced Input Validation**: Strict name validation (letters only, no numbers)
- **⭐ Hybrid Review System**: Star ratings (1-5) + optional comments (50 chars)
- **🚨 Alert System**: Automated alerts for overdue rentals and inactive vehicles
- **💾 Backup & Restore**: Complete data backup and restoration functionality
- **📅 Professional Calendar UI**: ASCII grid calendar for booking visualization
- **👨‍💼 Driver Module**: Complete driver management with earnings tracking
- **🎫 Promo Code System**: Discount codes with percentage-based savings
- **📊 Advanced Reporting**: Monthly revenue, top vehicles, export to CSV
- **🔍 Search & Filter**: Advanced search capabilities across all modules

### Core Functionality

- **Customer Management**: Registration, login, profile management with security
- **Admin Panel**: Full system administration with role-based access
- **Vehicle Management**: Add, update, view, and soft delete vehicles with ratings
- **Rental System**: Book vehicles by hour, day, or route-based trips
- **Driver Management**: Driver assignment, earnings tracking, performance metrics
- **Promo System**: Discount codes and promotional offers
- **Search & Filter**: Advanced search, filtering, and sorting
- **Reports & Analytics**: Generate business reports and analytics
- **Dashboard**: Real-time system overview with calendar view
- **Alert System**: Automated monitoring and notifications
- **Backup System**: Data backup and restoration capabilities

### Technical Features & Code Quality

- **Modular Architecture**: Clean separation with dedicated modules (alert, backup, driver, promo)
- **Linked List Data Structures**: Efficient dynamic data management
- **Persistent Storage**: CSV-based data persistence with backward compatibility
- **Input Validation**: Comprehensive error handling and validation
- **Memory Management**: Proper allocation, deallocation, and cleanup
- **Cross-Platform**: Works on Windows, Linux, and macOS
- **Security Features**: Password hashing, input sanitization, role-based access
- **Comprehensive Documentation**: Inline comments and function docs

### Advanced & Recent Features

- **Password Hashing**: DJB2 algorithm with backward compatibility for old plain text passwords
- **Name Validation**: Strict validation preventing numbers and symbols in customer names
- **Review System**: Integrated rating system with optional text comments
- **Alert Module**: Separate module for system monitoring and notifications
- **Backup Module**: Dedicated backup and restore functionality
- **Driver Module**: Complete driver management with advanced features
- **Promo Module**: Integrated promotional code system
- **Calendar UI**: Professional ASCII grid calendar for booking visualization

## 📁 Project Structure

```
RideMate--Your-Smart-Ride-Booking-Platform/
├── main.c              # Main application entry point
├── utils.h/c           # Utility functions, validation, and security
├── vehicle.h/c         # Vehicle and route management
├── customer.h/c        # Customer management with security
├── rental.h/c          # Rental system with reviews
├── driver.h/c          # Driver management module
├── promo.h/c           # Promo code management
├── alert.h/c           # Alert system for monitoring
├── backup.h/c          # Backup and restore functionality
├── search.h/c          # Search and filtering
├── dashboard.h/c       # System dashboard with calendar
├── reports.h/c         # Reporting and analytics
├── invoice.h/c         # Invoice and billing
├── vehicles.csv        # Vehicle data storage
├── customers.csv       # Customer data storage
├── rentals.csv         # Rental data storage with reviews
├── routes.csv          # Route data storage
├── promos.csv          # Promo code data
├── drivers.csv         # Driver data
├── invoices.csv        # Invoice data
├── data/               # All core CSV files
├── reports/            # Generated business reports
├── receipts/           # Generated receipts
└── README.md           # This file
```

## 🛠️ Build & Installation

### Prerequisites

- GCC compiler (MinGW for Windows, or gcc for Linux/macOS)
- Git (for cloning)

### Build Steps

1. **Clone the repository:**

   ```bash
   git clone https://github.com/tanjimislam04/RideMate--Your-Smart-Ride-Booking-Platform.git
   cd RideMate--Your-Smart-Ride-Booking-Platform
   ```

2. **Compile the project:**

   ```bash
   # Windows (complete compilation with all modules)
   gcc main.c utils.c vehicle.c customer.c rental.c driver.c promo.c alert.c backup.c search.c dashboard.c reports.c invoice.c -o RideMate.exe

   # Linux/macOS
   gcc main.c utils.c vehicle.c customer.c rental.c driver.c promo.c alert.c backup.c search.c dashboard.c reports.c invoice.c -o RideMate
   ```

3. **Run the application:**

   ```bash
   # Windows
   RideMate.exe

   # Linux/macOS
   ./RideMate
   ```

### Troubleshooting

- **Compilation Errors**: Ensure all source files are included in the compilation command
- **File Permissions**: Make sure CSV files are readable/writable
- **Memory Issues**: System includes proper memory management and cleanup
- **Platform Issues**: Tested on Windows, should work on Linux/macOS with gcc

## 🎯 How to Use

### First Time Setup

1. Run the executable
2. Choose option 2 to register as a new customer
3. Fill in your details (name must contain only letters and spaces)
4. Use option 3 to access admin panel (username: `admin`, password: `admin123`)

### Customer Features

1. **Registration & Login**: Secure registration with password hashing
2. **Profile Management**: Update personal details with validation
3. **Vehicle Booking**: Browse and book available vehicles
4. **Rental Management**: View and manage your rentals
5. **Rating System**: Rate vehicles and drivers after rental completion
6. **Invoice & Receipts**: View detailed invoices and receipts

### Admin Features

1. **Vehicle Management**:
   - Add, update, and manage vehicles
   - View vehicle ratings and reviews
   - Monitor vehicle availability

2. **Driver Management**:
   - Add and manage drivers
   - Track driver earnings and performance
   - Assign drivers to rentals

3. **Customer Management**:
   - View all registered customers
   - Monitor customer activity
   - Manage customer accounts

4. **Rental Management**:
   - View all system rentals
   - Complete or cancel rentals
   - Monitor rental status

5. **Promo Code Management**:
   - Create and manage discount codes
   - Set discount percentages
   - Monitor promo code usage

6. **Alert System**:
   - View overdue rentals
   - Monitor inactive vehicles
   - System health notifications

7. **Backup & Restore**:
   - Create complete system backups
   - Restore from backup files
   - Data migration support

8. **Reports & Analytics**:
   - Generate monthly revenue reports
   - View top performing vehicles
   - Export data to CSV files

9. **Dashboard**:
   - Real-time system overview
   - Calendar view for bookings
   - System statistics

## 🔐 Security Features

### Password Security
- **DJB2 Hashing Algorithm**: All passwords are hashed before storage
- **Backward Compatibility**: Old plain text passwords are automatically converted
- **Secure Storage**: No plain text passwords in the system

### Input Validation
- **Name Validation**: Only letters and spaces allowed (no numbers or symbols)
- **Email Validation**: Basic email format checking
- **Phone Validation**: Bangladesh phone format validation (+880)
- **Number Validation**: Comprehensive numeric input validation

### Access Control
- **Role-Based Access**: Separate customer and admin interfaces
- **Authentication**: Secure login system for both customers and admins
- **Session Management**: Proper session handling and validation

## 📊 Data Management

### CSV File Structure
- **Backward Compatibility**: System handles old CSV formats automatically
- **Data Migration**: Seamless upgrade from older versions
- **Error Recovery**: Robust error handling for corrupted data

### Backup System
- **Complete Backups**: All data files backed up together
- **Timestamped Backups**: Automatic timestamp for backup files
- **Restore Functionality**: Complete system restoration capability

## 🔧 Recent Improvements & Fixes

### Security Enhancements
- ✅ **Password Hashing**: Implemented DJB2 algorithm
- ✅ **Name Validation**: Strict validation preventing invalid characters
- ✅ **Input Sanitization**: Comprehensive input validation

### User Experience
- ✅ **Review System**: Star ratings with optional comments
- ✅ **Calendar UI**: Professional booking calendar
- ✅ **Alert System**: Automated notifications
- ✅ **Backup System**: Data protection and recovery

### Code Quality
- ✅ **Modular Design**: Separate modules for different functionalities
- ✅ **Error Handling**: Comprehensive error handling and recovery
- ✅ **Memory Management**: Proper allocation and cleanup
- ✅ **Documentation**: Complete inline documentation

### System Features
- ✅ **Driver Module**: Complete driver management
- ✅ **Promo System**: Discount code functionality
- ✅ **Advanced Reporting**: Business analytics and reports
- ✅ **Search & Filter**: Enhanced search capabilities

## 🚀 System Requirements

### Minimum Requirements
- **OS**: Windows 10/11, Linux, macOS
- **Memory**: 512MB RAM
- **Storage**: 100MB free space
- **Compiler**: GCC 4.8+ or MinGW

### Recommended Requirements
- **OS**: Windows 10/11, Ubuntu 18.04+, macOS 10.14+
- **Memory**: 1GB RAM
- **Storage**: 500MB free space
- **Compiler**: GCC 7.0+ or MinGW-w64

## 📈 Performance & Scalability

### Current Capabilities
- **Data Storage**: Unlimited CSV-based storage
- **User Management**: No limit on customers or vehicles
- **Rental Processing**: Efficient rental management
- **Reporting**: Fast report generation

### Optimization Features
- **Linked Lists**: Efficient data structure for dynamic data
- **Memory Management**: Proper cleanup and optimization
- **File I/O**: Optimized CSV reading and writing
- **Search Algorithms**: Fast search and filtering

## 🎯 Future Roadmap

### Completed Features ✅
- [x] Core rental management system
- [x] Customer and admin interfaces
- [x] Vehicle and driver management
- [x] Promo code system
- [x] Review and rating system
- [x] Alert and backup systems
- [x] Advanced reporting and analytics
- [x] Security features and validation
- [x] Professional UI improvements

### Planned Features 🔄
- [ ] Web interface development
- [ ] Mobile app integration
- [ ] Payment gateway integration
- [ ] Real-time notifications
- [ ] Advanced analytics dashboard
- [ ] Multi-language support
- [ ] API development
- [ ] Cloud storage integration

## 👨‍💻 Author & Contact

**Md. Tanjim Islam**  
Software Engineering Student  
Daffodil International University

## 📄 License & Contribution

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

## 📝 Educational Value

This project demonstrates advanced C programming concepts including:
- **Data Structures**: Linked lists, structs, enums
- **File I/O**: CSV reading/writing, data persistence
- **Memory Management**: Dynamic allocation, cleanup
- **Modular Design**: Header/source file organization
- **Security**: Password hashing, input validation
- **User Interface**: Console-based interactive menus
- **Error Handling**: Comprehensive error management

The codebase is designed to be educational and easily extensible for real-world applications.

---

**RideMate** - Making vehicle rental management simple, secure, and efficient! 🚗✨

*Built with ❤️ in C Programming Language*

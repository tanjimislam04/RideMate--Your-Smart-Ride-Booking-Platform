# RideMate: Your Smart Ride Booking Platform

## Description

RideMate is a comprehensive, console-based Vehicle Rental Management System built in C. It features a modular architecture with advanced data structures (linked lists) for efficient management of vehicles, customers, rentals, and routes. The system provides both customer and administrative interfaces with persistent data storage and robust error handling.

## 🚀 Features

### Core Functionality
- **Customer Management**: Registration, login, profile management
- **Admin Panel**: Complete system administration with role-based access
- **Vehicle Management**: Add, update, view, and soft delete vehicles
- **Rental System**: Book vehicles by hour, day, or route-based trips
- **Route Management**: Define and manage transportation routes
- **Search & Filter**: Advanced search and filtering capabilities
- **Reports & Analytics**: Generate business reports and analytics
- **Dashboard**: Real-time system overview and statistics

### Technical Features
- **Modular Architecture**: Clean separation of concerns with header/source files
- **Linked List Data Structures**: Efficient dynamic data management
- **Persistent Storage**: CSV-based data persistence
- **Input Validation**: Comprehensive error handling and validation
- **Memory Management**: Proper memory allocation and deallocation
- **Cross-Platform**: Works on Windows, Linux, and macOS

### Advanced Features
- **Multiple Rental Types**: Hourly, daily, and route-based rentals
- **Vehicle Categories**: Car, Motorcycle, Truck, Van
- **Status Tracking**: Active, completed, and cancelled rentals
- **Revenue Reports**: Monthly revenue analysis
- **Usage Analytics**: Top rented vehicles tracking
- **Soft Delete**: Data preservation with status-based deletion

## 📁 Project Structure

```
RideMate--Your-Smart-Ride-Booking-Platform/
├── main.c              # Main application entry point
├── utils.h/c           # Utility functions and helpers
├── vehicle.h/c         # Vehicle and route management
├── customer.h/c        # Customer management
├── rental.h/c          # Rental system
├── search.h/c          # Search and filtering
├── dashboard.h/c       # System dashboard
├── reports.h/c         # Reporting and analytics
├── vehicles.csv        # Vehicle data storage
├── customers.csv       # Customer data storage
├── rentals.csv         # Rental data storage
├── routes.csv          # Route data storage
└── README.md           # This file
```

## 🛠️ Build Instructions

### Prerequisites
- GCC compiler (MinGW for Windows)
- Git (for cloning)

### Build Steps

1. **Clone the repository:**
   ```bash
   git clone https://github.com/tanjimislam04/RideMate--Your-Smart-Ride-Booking-Platform.git
   cd RideMate--Your-Smart-Ride-Booking-Platform
   ```

2. **Compile the project:**
   ```bash
   # Compile all source files
   gcc -c *.c -Wall -Wextra
   
   # Link object files to create executable
   gcc *.o -o RideMate.exe
   ```

3. **Run the application:**
   ```bash
   # Windows
   RideMate.exe
   
   # Linux/macOS
   ./RideMate
   ```

### Alternative Build (Single Command)
```bash
gcc main.c utils.c vehicle.c customer.c rental.c search.c dashboard.c reports.c -o RideMate.exe
```

## 🎯 How to Use

### First Time Setup
1. Run the executable
2. Choose option 2 to register as a new customer
3. Fill in your details (name, username, password, email, phone)
4. Use option 3 to access admin panel (username: `admin`, password: `admin123`)

### Customer Features
1. **Login**: Use your registered username and password
2. **View Profile**: Check your account information
3. **Update Profile**: Modify your personal details
4. **Browse Vehicles**: View available vehicles for rent
5. **Book Rental**: Choose rental type (hourly/daily/route)
6. **View Rentals**: Check your rental history

### Admin Features
1. **Vehicle Management**:
   - Add new vehicles with specifications
   - Update vehicle rates and availability
   - List all vehicles (active/inactive)
   - Soft delete vehicles

2. **Route Management**:
   - Add new transportation routes
   - Set base fares and estimated times
   - View all available routes

3. **Customer Management**:
   - View all registered customers
   - Soft delete customer accounts
   - Export/import customer data

4. **Rental Management**:
   - View all system rentals
   - Complete or cancel rentals
   - Monitor rental status

5. **Search & Filter**:
   - Search vehicles by make/model
   - Filter by vehicle type or price
   - Sort vehicles by various criteria
   - Find rentals by customer ID

6. **Dashboard**:
   - View system statistics
   - Monitor active rentals
   - Check vehicle availability

7. **Reports**:
   - Generate monthly revenue reports
   - View top rented vehicles
   - Export data to CSV files

## 🔧 Recent Fixes and Improvements

### Compilation Issues Resolved
- ✅ Fixed missing function declarations in `utils.h`
- ✅ Implemented missing utility functions (`generateUniqueId`, `getInput`, `isValidNumber`)
- ✅ Resolved MAX_STRING redefinition conflicts
- ✅ Fixed function signature mismatches in search module
- ✅ Added missing memory management functions (`freeVehicleList`, `freeCustomerList`, `freeRentalList`)
- ✅ Implemented missing display functions (`displayCustomerProfile`, `listAllRentals`)
- ✅ Fixed format specifier issues in vehicle display
- ✅ Resolved const qualifier warnings

### Code Quality Improvements
- ✅ Enhanced error handling and input validation
- ✅ Improved memory management with proper cleanup
- ✅ Added comprehensive function documentation
- ✅ Standardized code formatting and structure

## 📊 Data Storage

The system uses CSV files for persistent data storage:
- `vehicles.csv`: Vehicle information and specifications
- `customers.csv`: Customer registration and profile data
- `rentals.csv`: Rental transaction records
- `routes.csv`: Transportation route definitions

## 🔐 Security Features

- Password hashing for customer accounts
- Admin authentication system
- Input validation and sanitization
- Role-based access control

## 🚀 Roadmap

### Completed ✅
- [x] Core rental management system
- [x] Customer and admin interfaces
- [x] Vehicle and route management
- [x] Search and filtering capabilities
- [x] Reporting and analytics
- [x] Persistent data storage
- [x] Input validation and error handling
- [x] Memory management and cleanup

### Planned Features 🔄
- [ ] Enhanced authentication system
- [ ] Payment processing integration
- [ ] Real-time notifications
- [ ] Mobile app interface
- [ ] Advanced analytics dashboard
- [ ] Multi-language support
- [ ] API integration capabilities

## 👨‍💻 Author

**Md. Tanjim Islam**  
Software Engineering Student  
Daffodil International University  


## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

## 📝 Note

This project is developed for academic and learning purposes. It demonstrates advanced C programming concepts including data structures, file I/O, memory management, and modular software design. The codebase is designed to be educational and easily extensible for real-world applications.

---

**RideMate** - Making vehicle rental management simple and efficient! 🚗✨

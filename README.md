# title: "RideMate: Your Smart Ride Booking Platform"

# description:

RideMate is a modular, console-based Vehicle Rental Management System built in C.
It uses advanced data structures (linked lists) for dynamic and efficient management of vehicles, customers, and rentals.
Menu-driven, user-friendly, and highly extendable for real-world learning.

# core_version:

- Customer registration & login
- Admin login
- Vehicle management (add, view, update, soft delete)
- Customer management (view, update, soft delete)
- Rental management (book, view history)
- Persistent data save/load (using files)
- Modular C code (header/source file split)
- Advanced data structures (linked lists) for storage
- Input validation and basic error handling

# features:

- Dynamic data storage using linked lists
- Menu-driven, real-world project structure
- Persistent file save/load
- Customer & admin modules
- Scalable & extendable codebase

# build_instructions:

- Clone this repository:
- git clone https://github.com/tanjimislam04/RideMate--Your-Smart-Ride-Booking-Platform.git
- cd RideMate--Your-Smart-Ride-Booking-Platform
- Compile (Linux/Mac):
- gcc main.c utils.c vehicle.c customer.c rental.c -o ridemate
- Run:
- ./ridemate
- "(Windows user? Use CodeBlocks, Dev-C++, or compatible C IDE)"

# how_to_use:

- On start, select Customer or Admin.
- Register or log in with your credentials.
- Customers can view/update their profile, browse vehicles, and book rentals.
- Admins can manage vehicles, customers, and view rental history.
- Data is saved automatically in files (vehicles.dat, customers.dat, rentals.dat, etc.)

# roadmap:

- "[x] Core version with linked list-based storage"
- "[x] Modular structure & menu-driven UI"
- "[ ] Strict input validation & error handling"
- "[ ] Role-based authentication (Admin/Customer/Driver)"
- "[ ] Advanced features (CSV export/import, search/filter, analytics, soft delete)"
- "[ ] Pro-level features (promo code, calendar view, graphical output, etc.)"

# author:

- "Md. Tanjim Islam"
- "Software Engineering Student, Daffodil International University"

# note:

This project is for academic and learning purposes. More features and advanced data structures will be added step by step.

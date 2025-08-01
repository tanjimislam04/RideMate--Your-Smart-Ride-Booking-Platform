# RideMate: Your Smart Ride Booking Platform

RideMate is a console-based Vehicle Rental Management System built in C, using structures, pointers, and text files.
Easily manage customers, bookings, vehicles, pricing, and routes—all from a simple menu-driven interface.
Perfect for learning core C programming concepts and building real-world solutions.
The project follows real-world code structure (with modules) and can be easily extended!

# Features (Core Version)

      - Customer registration & login
      - Admin login
      - Vehicle management (add, view, update)
      - Customer management (view, update)
      - Rental management (book, view rentals)
      - Persistent data save/load (using files)
      - Structured C code with header/source split

# Build Instructions

      - "Clone this repository:"
      - "git clone https://github.com/tanjimislam04/RideMate--Your-Smart-Ride-Booking-Platform.git"
      - "cd RideMate--Your-Smart-Ride-Booking-Platform"
      - "Compile (Linux/Mac):"
      - "gcc main.c utils.c vehicle.c customer.c rental.c -o ridemate"
      - "Run:"
      - "./ridemate"
      - "(Windows user? Use CodeBlocks, Dev-C++, or compatible C IDE)"

# How to Use

      - On start, select Customer or Admin.
      - Follow menu prompts to register, log in, manage vehicles/customers/rentals.
      - Data is saved automatically in local files (`vehicles.tmp`, `customers.tmp`, etc.)

# Roadmap

    checkboxes:
      - "[x] Core structure & main menu"
      - "[ ] Input validation (strict)"
      - "[ ] Role-based authentication"
      - "[ ] Modular code split (.c/.h files)"
      - "[ ] Advanced features (search/filter, analytics, soft delete, etc.)"
      - "[ ] Pro-level features (promo code, calendar, CSV export, etc.)"

# Author

      - "[Md. Tanjim Islam]"
      - "Software Engineering Student, Daffodil International University"

# Note

      This project is for academic and learning purposes. More features and documentation will be added step by step.

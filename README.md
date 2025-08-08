# RideMate: Your Smart Ride Booking Platform

## Description

RideMate is a modular, console-based Vehicle Rental Management System built in C.  
It uses advanced data structures (linked lists) for dynamic and efficient management of vehicles, customers, and rentals.  
Menu-driven, user-friendly, and highly extendable for real-world learning.

## Core Version

- Customer registration & login
- Admin login
- Vehicle management (add, view, update, soft delete)
- Customer management (view, update, soft delete)
- Rental management (book, view history)
- Persistent data save/load (using files)
- Modular C code (header/source file split)
- Advanced data structures (linked lists) for storage
- Input validation and basic error handling

## Features

- Dynamic data storage using linked lists
- Menu-driven, real-world project structure
- Persistent file save/load
- Customer & admin modules
- Scalable & extendable codebase

## Build Instructions

- Clone this repository:
  ```sh
  git clone https://github.com/tanjimislam04/RideMate--Your-Smart-Ride-Booking-Platform.git
  cd RideMate--Your-Smart-Ride-Booking-Platform
  ```
- Compile (Linux/Mac):
  ```sh
  gcc main.c utils.c vehicle.c customer.c rental.c -o ridemate
  ```
- Run:
  ```sh
  ./ridemate
  ```
- **Windows users:**  
  Use CodeBlocks, Dev-C++, or any compatible C IDE.  
  Or, in Command Prompt (after compiling with GCC/MinGW):
  ```
  ridemate.exe
  ```

## How to Use

- On start, select Customer or Admin.
- Register or log in with your credentials.
- Customers can view/update their profile, browse vehicles, and book rentals.
- Admins can manage vehicles, customers, and view rental history.
- Data is saved automatically in files (`vehicles.csv`, `customers.csv`, `rentals.csv`, etc.)

## Roadmap

- [x] Core version with linked list-based storage
- [x] Modular structure & menu-driven UI
- [x] Strict input validation & error handling
- [ ] Role-based authentication (Admin/Customer/Driver)
- [ ] Advanced features (CSV export/import, search/filter, analytics, soft delete)
- [ ] Pro-level features (promo code, calendar view, graphical output, etc.)

## Author

- Md. Tanjim Islam  
  Software Engineering Student, Daffodil International University

## Note

This project is for academic and learning purposes. More features and advanced data structures will be added step by step.

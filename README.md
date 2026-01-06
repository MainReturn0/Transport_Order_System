# Transport Order System

A C++ demonstration of order processing with transport selection logic, illustrating design patterns and object-oriented principles.

## Overview

This system processes orders and automatically selects the appropriate transport method (Truck, Ship, or Air) based on order characteristics like weight, distance, and urgency.

## Components

### Transport Types
- **TruckTransport**: Ground delivery with route planning and heavy equipment options
- **ShipTransport**: Maritime shipping with port reservations and customs clearance
- **AirTransport**: Fast air delivery with express handling option

### Core Classes
- **OrderDetails**: Data container for order information (ID, weight, distance, urgency)
- **ITransport**: Abstract interface for all transport implementations
- **OrderManager**: Handles order processing and transport selection

## Transport Selection Rules

1. **Air Transport**: Selected for urgent, lightweight (<20kg), long-distance (>500km) orders
2. **Ship Transport**: Selected for extreme weight (>1000kg) or very long distances (>2000km)
3. **Truck Transport**: Default option for all other orders

## Design Notes

This implementation embeds the transport selection and creation logic directly in the `OrderManager` class (non-factory approach). The manager is responsible for both:
- Business logic (processing orders)
- Construction logic (selecting and creating transports)

## Building and Running

```bash
# Compile
g++ -std=c++17 Order.cpp -o order_system

# Run
./order_system
```

## Example Usage

```cpp
OrderManager manager;

// Process various orders
manager.process_order({1, 15.0, 600.0, true});   // Air (urgent + light + long)
manager.process_order({2, 1200.0, 2500.0, false}); // Ship (heavy + very long)
manager.process_order({3, 150.0, 300.0, false});  // Truck (default)

// Display results
manager.print_records();
```

## Requirements

- C++17 or later
- Standard library support for `<memory>`, `<string>`, `<vector>`

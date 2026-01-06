# Logistics Command Center

A small web app that combines a C++ transport decision engine with a Python Flask UI. Orders are processed via a shared C++ library using a factory pattern, and results are displayed in a modern frontend.

## Overview

- Backend: C++17 shared library with `TransportFactory` selecting `Truck`, `Ship`, or `Air` transports based on order details.
- Web: Flask app that loads the C++ library via `ctypes` and exposes a JSON API.
- Frontend: HTML + CSS with a simple dashboard to submit orders and view recent activity.

Key files:

- [order_logic.cpp](order_logic.cpp): Transport classes, factory, and `extern "C"` interface (`add_order`, `get_orders_log`, `reset_system`).
- [Factory.py](Factory.py): Flask server; dynamically loads `logistics` library and serves routes.
- [templates/Factory.html](templates/Factory.html): UI page with form and results table.
- [static/Factory.css](static/Factory.css)

## Architecture

- `TransportFactory` centralizes transport selection logic and returns an `ITransport` implementation.
- `OrderManager` stores processed orders and exposes a summary string.
- A small C interface (`extern "C"`) allows Python to call C++ without binding generators:
  - `void add_order(int id, double weight, double distance, bool urgent)`
  - `const char* get_orders_log()`
  - `void reset_system()`
- The Flask app resolves the correct shared library name by OS:
  - macOS: `logistics.dylib`
  - Linux: `logistics.so`
  - Windows: `logistics.dll` (not verified in this repo)

## Transport Selection Rules

- Air: urgent AND weight < 20kg AND distance > 500km
- Ship: distance > 2000km OR weight > 1000kg
- Truck: default; route time scales with distance, urgency slightly reduces time; heavy threshold is 200kg

Implemented in [order_logic.cpp](order_logic.cpp) under `Config` and `TransportFactory::create_transport()`.

## Build the C++ Library

Place the built library next to [Factory.py](Factory.py).

macOS:

```bash
cd /Volumes/General/Factory_Singleton
g++ -std=c++17 -fPIC -shared order_logic.cpp -o logistics.dylib
```

Linux:

```bash
cd /Volumes/General/Factory_Singleton
g++ -std=c++17 -fPIC -shared order_logic.cpp -o logistics.so
```

Note: Windows builds may require export decorations or a `.def` file; this repo targets macOS/Linux.

## Run the Web App

```bash
cd /Volumes/General/Factory_Singleton
python3 -m venv .venv
source .venv/bin/activate
pip install flask
python Factory.py
```


## API

- Endpoint: `POST /process_order`
- Body (JSON):
  - `weight`: number (kg)
  - `distance`: number (km)
  - `urgent`: string ("true" | "false")

Example request:

```json
{
  "weight": 15.0,
  "distance": 600.0,
  "urgent": "true"
}
```

Typical response:

```json
{
  "id": 1234,
  "type": "air",
  "info": "Air (Express: Yes)",
  "eta": "Air: 1 day (Express)",
  "weight": 15.0,
  "distance": 600.0,
  "urgent": true
}
```

Transport badges and ETA are derived from the C++ engine output parsed by [Factory.py](Factory.py).

## Notes

- Ensure the `logistics` shared library is built and resides alongside [Factory.py](Factory.py) before running.
- The UI references optional images (`/static/air.jpg`, `/static/ship.jpg`, `/static/truck.jpg`). Add these under `static/` or adjust [templates/Factory.html](templates/Factory.html).
- The server currently resets the C++ manager per request with `lib.reset_system()`; remove or adapt for multi-order sessions.

## License

This repository is intended for demonstration and educational purposes.

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

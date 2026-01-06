# Logistics Command Center

## 1. Executive Summary

The Logistics Command Center is a web-based dashboard designed to simulate and process high-volume logistics orders. It determines the optimal transport method (Air, Ship, or Truck) based on weight, distance, and urgency.

The project demonstrates a **High-Performance Hybrid Architecture**:

- **Core Logic**: Implemented in C++ for performance and type safety.
- **Backend API**: Managed by Python (Flask) for rapid development and ease of routing.
- **Frontend**: Built with HTML5/CSS3 & JavaScript (AJAX) for a responsive, non-blocking user experience.

## 2. System Architecture

The application follows a **3-Tier Architecture** where the frontend communicates with the backend via RESTful APIs, and the backend delegates heavy computation to a compiled C++ shared library.

### Data Flow:

1. **User Input**: The user submits order details via the HTML Dashboard.
2. **Async Request**: JavaScript (fetch) sends the data as JSON to the Python Flask server.
3. **Interop Layer**: Python uses the `ctypes` library to load the compiled C++ Shared Object (`.so`).
4. **Core Processing**: C++ instantiates the appropriate Transport class (Factory Pattern), calculates the ETA, and returns a formatted string pointer.
5. **Response**: Python parses the C++ result and returns a JSON response to the browser.
6. **DOM Update**: JavaScript dynamically injects the new record into the table without a page reload.

## 3. Technology Stack

| Component | Technology | Role |
|-----------|-----------|------|
| Core Logic | C++ (C++14) | Handles business rules, Transport classes, and calculations. |
| Compilation | Clang++ | Compiles C++ into a position-independent shared library (`logistics.so`). |
| Backend | Python 3 (Flask) | Web server, API routing, and data marshaling. |
| Interface | ctypes | Python library for calling functions in DLLs/Shared Libraries. |
| Frontend | HTML5 / CSS3 | Semantic markup and responsive styling. |
| Scripting | Vanilla JavaScript | Handles AJAX requests and DOM manipulation. |

## 4. Design Patterns & Principles

### A. Object-Oriented Programming (OOP)

The C++ core relies heavily on OOP to ensure scalability.

- **Polymorphism**: An abstract base class `ITransport` defines the interface (`calculate_delivery_time`, `info`).
- **Inheritance**: Specific implementations (`TruckTransport`, `ShipTransport`, `AirTransport`) inherit from the base class.

### B. Factory Pattern (Simplified)

The system uses a creation logic (inside `OrderManager` or `TransportFactory`) to instantiate the correct class based on input criteria (e.g., "If urgent and light, create AirTransport"). This decouples object creation from the main execution flow.

### C. Singleton (Conceptual)

The C++ wrapper uses a static/global instance of the `OrderManager` to persist state (the list of records) between different Python calls.

## 5. Key Implementation Details

### The C++ Interface (`extern "C"`)

To allow Python to "see" the C++ functions, we wrapped the entry points in an `extern "C"` block. This prevents **Name Mangling**, ensuring the function names remain standard in the compiled binary.

```cpp
extern "C" {
    void add_order(int id, double weight, double distance, bool urgent) {
        // Bridges C-style inputs to C++ Objects
        manager.process(OrderDetails{id, weight, distance, urgent});
    }
}
```

### The Python Adapter (`ctypes`)

We specifically defined `argtypes` and `restype` in Python to prevent memory errors when passing data types (like `int` vs `float`) across the language barrier.

```python
lib.add_order.argtypes = [ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_bool]
```

## 6. Installation & Setup

### Prerequisites:
- Python 3
- Flask
- A C++ Compiler (Clang or GCC)

### Clone/Organize:

```
/Factory_Singleton
   ├── Factory.py          (Flask Server)
   ├── order_logic.cpp     (C++ Source)
   ├── logistics.so        (Compiled shared library)
   ├── templates/          (HTML)
   │   └── Factory.html
   └── static/             (CSS)
       └── Factory.css
```

### Compile C++ Logic:

```bash
clang++ -shared -std=c++14 -fPIC -o logistics.so order_logic.cpp
```

### Install Python Dependencies:

```bash
pip install flask
```

### Run Server:

```bash
python3 Factory.py
```

### Access:
Open browser to `http://127.0.0.1:5000`.

## 7. Transport Selection Logic

The system automatically selects the optimal transport method based on the following rules:

### ✈️ Air Transport
- **Conditions**: Urgent delivery, weight < 20 kg, distance > 500 km
- **ETA**: 1 day (Express) or 2 days
- **Best for**: Time-sensitive, lightweight shipments

### 🚢 Ship Transport
- **Conditions**: Distance > 2000 km OR weight > 1000 kg
- **ETA**: 10-13+ days (includes clearance)
- **Best for**: Heavy cargo or international long-distance

### 🚚 Truck Transport
- **Conditions**: Default for all other cases
- **ETA**: 1-3+ days (depends on route and load)
- **Best for**: Standard domestic deliveries

## 8. API Endpoints

### `GET /`
Returns the main dashboard HTML page.

### `POST /process_order`
Processes a new logistics order.

**Request Body (JSON)**:
```json
{
  "weight": 150.5,
  "distance": 600,
  "urgent": "true"
}
```

**Response (JSON)**:
```json
{
  "id": 1234,
  "type": "truck",
  "info": "Truck (Route: 42m)",
  "eta": "Truck: 2 days",
  "weight": 150.5,
  "distance": 600,
  "urgent": true
}
```

## 9. Project Structure Details

### `order_logic.cpp`
Contains the C++ implementation with:
- Transport interface and classes (`ITransport`, `TruckTransport`, `ShipTransport`, `AirTransport`)
- `TransportFactory` for creating appropriate transport instances
- `OrderManager` for managing order records
- Extern C functions for Python interoperability

### `Factory.py`
Flask application that:
- Loads the compiled C++ library using `ctypes`
- Defines API routes for processing orders
- Handles JSON serialization/deserialization
- Bridges Python and C++ data types

### `templates/Factory.html`
Frontend interface featuring:
- Order input form (weight, distance, priority)
- Real-time order processing with AJAX
- Dynamic table updates without page reload
- Visual transport selection logic display

### `static/Factory.css`
Responsive styling with:
- Color-coded transport types
- Modern card-based layout
- Mobile-friendly design
- Smooth animations and transitions

## 10. Features

✅ **No Page Reloads**: AJAX-based communication ensures a seamless user experience  
✅ **High Performance**: C++ handles computationally intensive logic  
✅ **Flexible Architecture**: Easy to add new transport types through the Factory pattern  
✅ **Type Safety**: Strong typing in C++ prevents runtime errors  
✅ **Scalable**: Separation of concerns allows independent scaling of components  
✅ **Visual Feedback**: Color-coded transport types for easy identification

## 11. Future Enhancements

- Add database persistence for order history
- Implement user authentication and authorization
- Add real-time tracking simulation
- Support for multiple currencies and units
- REST API documentation with Swagger/OpenAPI
- Containerization with Docker
- Unit tests for C++ and Python components

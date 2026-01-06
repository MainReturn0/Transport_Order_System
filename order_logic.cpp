// Instead of using java script, we will implement the order management system in C++. Therefore we wont use the main function cause we will use python to call the c++ code. so we will warp the logic in exter C function so that python can call it. To make this scalable, we move the decision logic (Truck vs. Ship vs. Air) out of the OrderManager and into a dedicated TransportFactory.

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

// Use a simplified namespace scope to keep code clean
using namespace std;

// ==========================================
// 1. Constants & Configuration ⚙️
// ==========================================
namespace Config
{
  constexpr double AIR_MAX_WEIGHT = 20.0;
  constexpr double AIR_MIN_DIST = 500.0;
  constexpr double SHIP_MIN_DIST = 2000.0;
  constexpr double SHIP_MAX_WEIGHT = 1000.0;
  constexpr double TRUCK_HEAVY_THRESHOLD = 200.0;
}

struct OrderDetails
{
  int id;
  double weight_kg;
  double distance_km;
  bool urgent;
};

// ==========================================
// 2. Transport Interface & Classes 🚚 🚢 ✈️
// ==========================================

class ITransport
{
public:
  virtual ~ITransport() = default;
  virtual string calculate_delivery_time() const = 0;
  virtual string info() const = 0;
};

class TruckTransport : public ITransport
{
  double route_minutes_;
  bool heavy_load_;

public:
  TruckTransport(double minutes, bool heavy)
      : route_minutes_(minutes), heavy_load_(heavy) {}

  string calculate_delivery_time() const override
  {
    int days = 1 + static_cast<int>(route_minutes_ / 60);
    if (heavy_load_)
      days += 1;
    return "Truck: " + to_string(days) + " days";
  }

  string info() const override
  {
    return "Truck (Route: " + to_string(static_cast<int>(route_minutes_)) + "m)";
  }
};

class ShipTransport : public ITransport
{
  bool reserved_;
  int clearance_days_;

public:
  ShipTransport(bool reserved, int clearance)
      : reserved_(reserved), clearance_days_(clearance) {}

  string calculate_delivery_time() const override
  {
    int days = 10 + clearance_days_;
    if (!reserved_)
      days += 3;
    return "Ship: " + to_string(days) + " days";
  }

  string info() const override
  {
    return "Ship (Reserved: " + string(reserved_ ? "Yes" : "No") + ")";
  }
};

class AirTransport : public ITransport
{
  bool express_;

public:
  explicit AirTransport(bool express) : express_(express) {}

  string calculate_delivery_time() const override
  {
    return express_ ? "Air: 1 day (Express)" : "Air: 2 days";
  }

  string info() const override
  {
    return "Air (Express: " + string(express_ ? "Yes" : "No") + ")";
  }
};

// ==========================================
// Transport Factory (The Scalable Part) later, we only change the Factory, not the entire Manager.
// ==========================================

class TransportFactory
{
public:
  static unique_ptr<ITransport> create_transport(const OrderDetails &order)
  {
    // Rule 1: Air Transport
    if (order.urgent && order.weight_kg < Config::AIR_MAX_WEIGHT && order.distance_km > Config::AIR_MIN_DIST)
    {
      return make_unique<AirTransport>(/*express=*/true);
    }

    // Rule 2: Ship Transport
    if (order.distance_km > Config::SHIP_MIN_DIST || order.weight_kg > Config::SHIP_MAX_WEIGHT)
    {
      return make_unique<ShipTransport>(/*reserved=*/true, /*clearance=*/2);
    }

    // Rule 3: Truck Transport (Default)
    double base_mins = 30.0 + (order.distance_km / 50.0);
    if (order.urgent)
      base_mins *= 0.8;

    bool is_heavy = order.weight_kg > Config::TRUCK_HEAVY_THRESHOLD;
    return make_unique<TruckTransport>(base_mins, is_heavy);
  }
};

// ==========================================
// Order Manager
// ==========================================

class OrderManager
{
  struct Record
  {
    int id;
    unique_ptr<ITransport> transport;
  };
  vector<Record> records_;

public:
  void process(const OrderDetails &details)
  {
    auto transport = TransportFactory::create_transport(details);
    records_.push_back({details.id, std::move(transport)});
  }

  // Generates a summary string for Python to read
  string get_summary() const
  {
    stringstream ss;
    for (const auto &r : records_)
    {
      ss << "[Order #" << r.id << "] "
         << r.transport->info() << " -> ETA: "
         << r.transport->calculate_delivery_time() << "\n";
    }
    return ss.str();
  }

  void clear() { records_.clear(); }
};

// ==========================================
// C Interface for Python (Extern C)
// ==========================================

// Global instances to persist state between Python calls
static OrderManager manager_instance;
static string last_output_buffer;

extern "C"
{
  // Add an order to the system
  void add_order(int id, double weight, double distance, bool urgent)
  {
    OrderDetails d{id, weight, distance, urgent};
    manager_instance.process(d);
  }

  // Get the formatted log of all orders
  const char *get_orders_log()
  {
    last_output_buffer = manager_instance.get_summary();
    return last_output_buffer.c_str();
  }

  // Clear memory/reset Allows Python to clear the list without restarting the process.
  void reset_system()
  {
    manager_instance.clear();
    last_output_buffer.clear();
  }
}
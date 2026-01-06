#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// Data container for order information
struct OrderDetails {
  int id;
  double weight_kg;
  double distance_km;
  bool urgent;
};

// Abstract interface for all transport types
class ITransport {
 public:
  virtual ~ITransport() = default;

  virtual string calculate_delivery_time() const = 0;
  virtual string info() const = 0;
};

// Truck transport implementation with route planning and equipment settings
class TruckTransport : public ITransport {
 public:
  TruckTransport(double route_planning_minutes, bool heavy_load_equipment)
      : route_planning_minutes_(route_planning_minutes),
        heavy_load_equipment_(heavy_load_equipment) {}

  string calculate_delivery_time() const override {
    int days = 1 + static_cast<int>(route_planning_minutes_ / 60);
    if (heavy_load_equipment_) days += 1;
    return "Truck: " + to_string(days) + " days";
  }

  string info() const override {
    return "TruckTransport(route_planning=" + to_string(route_planning_minutes_) +
           "min, heavy_eq=" + (heavy_load_equipment_ ? "yes" : "no") + ")";
  }

 private:
  double route_planning_minutes_;
  bool heavy_load_equipment_;
};

// Ship transport implementation with port reservations and customs clearance
class ShipTransport : public ITransport {
 public:
  ShipTransport(bool port_slot_reserved, int expected_clearance_days)
      : port_slot_reserved_(port_slot_reserved),
        expected_clearance_days_(expected_clearance_days) {}

  string calculate_delivery_time() const override {
    int days = 10 + expected_clearance_days_;
    if (!port_slot_reserved_) days += 3;
    return "Ship: " + to_string(days) + " days";
  }

  string info() const override {
    return string("ShipTransport(port_slot=") + (port_slot_reserved_ ? "yes" : "no") +
           ", clearance=" + to_string(expected_clearance_days_) + "d)";
  }

 private:
  bool port_slot_reserved_;
  int expected_clearance_days_;
};

// Air transport implementation with express handling option
class AirTransport : public ITransport {
 public:
  AirTransport(bool express_handling) : express_handling_(express_handling) {}

  string calculate_delivery_time() const override {
    return express_handling_ ? "Air: 1 day (express)" : "Air: 2 days";
  }

  string info() const override {
     return string("AirTransport(express=") + (express_handling_ ? "yes" : "no") + ")";
  }

 private:
  bool express_handling_;
};

// OrderManager: handles order processing and transport selection
// Note: Creation logic is embedded here (no factory pattern used)
class OrderManager {
 public:
  void process_order(const OrderDetails& details) {
    unique_ptr<ITransport> transport = create_transport_for(details);
    records_.push_back({details.id, details, move(transport)});
  }

  void print_records() const {
    for (const auto& r : records_) {
      cout << "Order #" << r.id << " | weight=" << r.details.weight_kg
           << "kg | dist=" << r.details.distance_km << "km | urgent=" << (r.details.urgent ? "yes" : "no")
           << "\n   -> " << r.transport->info()
           << " | ETA: " << r.transport->calculate_delivery_time() << "\n";
    }
  }

 private:
  struct Record {
    int id;
    OrderDetails details;
    unique_ptr<ITransport> transport;
  };

  vector<Record> records_;

  // Selects and creates appropriate transport based on order characteristics
  unique_ptr<ITransport> create_transport_for(const OrderDetails& order) const {
    // Urgent, light, long distance: use air
    if (order.urgent && order.weight_kg < 20.0 && order.distance_km > 500.0) {
      bool express = true;
      return make_unique<AirTransport>(express);
    }

    // Extreme weight or long distance: use ship
    if (order.distance_km > 2000.0 || order.weight_kg > 1000.0) {
      bool port_slot_reserved = try_reserve_port_slot(order);
      int clearance_days = estimate_customs_clearance(order);
      return make_unique<ShipTransport>(port_slot_reserved, clearance_days);
    }

    // Default: use truck
    double route_planning_minutes = plan_route_minutes(order);
    bool heavy_eq = (order.weight_kg > 200.0);
    return make_unique<TruckTransport>(route_planning_minutes, heavy_eq);
  }

  // Helper methods for transport-specific calculations
  static double plan_route_minutes(const OrderDetails& order) {
    double base = 30.0;
    base += order.distance_km / 50.0;
    if (order.urgent) base *= 0.8;
    return base;
  }

  static bool try_reserve_port_slot(const OrderDetails& order) {
    return true;
  }

  static int estimate_customs_clearance(const OrderDetails& order) {
    return 2;
  }
};
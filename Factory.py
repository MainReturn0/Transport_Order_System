import ctypes
import os
import re
import random
from flask import Flask, render_template, request, jsonify
import sys

app = Flask(__name__)

# --- Load C++ Library ---
_lib_base_name = "logistics"
if os.name == "nt":
    _lib_filename = f"{_lib_base_name}.dll"
elif sys.platform == "darwin":
    _lib_filename = f"{_lib_base_name}.dylib"
else:
    # Preserve existing behavior on Unix-like systems
    _lib_filename = f"{_lib_base_name}.so"

_here = os.path.dirname(os.path.abspath(__file__))
lib_path = os.path.join(_here, _lib_filename)
lib = ctypes.CDLL(lib_path)

lib.add_order.argtypes = [ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_bool]
lib.get_orders_log.argtypes = []
lib.get_orders_log.restype = ctypes.c_char_p
lib.reset_system.argtypes = []


@app.route("/")
def index():
    return render_template("Factory.html")


@app.route("/process_order", methods=["POST"])
def process_order():
    data = request.json  # Read JSON data sent by JavaScript

    weight = float(data.get("weight"))
    distance = float(data.get("distance"))
    urgent = data.get("urgent") == "true"

    order_id = random.randint(1000, 9999)

    # Call C++ Logic
    lib.reset_system()
    lib.add_order(order_id, weight, distance, urgent)

    raw_bytes = lib.get_orders_log()
    log_str = raw_bytes.decode("utf-8").strip()

    # Parse Result
    match = re.search(r"\[Order #\d+\] (.*?) -> ETA: (.*)", log_str)

    response = {"id": order_id}

    if match:
        info = match.group(1)
        eta = match.group(2)

        css_type = "truck"
        if "Air" in info:
            css_type = "air"
        elif "Ship" in info:
            css_type = "ship"

        response.update(
            {
                "type": css_type,
                "info": info,
                "eta": eta,
                "weight": weight,
                "distance": distance,
                "urgent": urgent,
            }
        )
    else:
        response.update({"type": "truck", "info": log_str, "eta": "Error"})

    return jsonify(response)  # Return DATA only (No page reload!)


if __name__ == "__main__":
    app.run(debug=True, port=5000)

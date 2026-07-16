import sys
print(sys.executable)
print(sys.version)

from flask import Flask, request
from datetime import datetime
import csv
import os

from ba63usb import BA63USB

app = Flask(__name__)

CSV_FILE = "weather.csv"

devices = BA63USB.get()

if not devices:
    print("No BA63 display found.")
    display = None
else:
    display = BA63USB(devices[0]["path"])


# Create the CSV with a header if it doesn't exist
if not os.path.exists(CSV_FILE):
    with open(CSV_FILE, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([
            "timestamp",
            "temperature",
            "humidity",
            "pressure",
            "light"
        ])

@app.route("/weather", methods=["POST"])
def weather():
    data = request.get_json()

    if data is None:
        return "Invalid JSON", 400

    print("Received:", data)

    display.clear()
    display.set_cursor(1, 1)
    display.print(f" T:{data['temperature']:.1f}C")
    display.print(f" H:{data['humidity']:.0f}%")
    display.print(f" P:{data['pressure']:.0f}%")
    display.print(f" L:{data['light']:.0f}%")

    with open(CSV_FILE, "a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([
            datetime.now().isoformat(),
            data["temperature"],
            data["humidity"],
            data["pressure"],
            data["light"]
        ])

    return "OK", 200

@app.route("/")
def index():
    return "Weather station is running."

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
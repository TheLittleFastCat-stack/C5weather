from flask import Flask, request
from datetime import datetime
import csv
import os

app = Flask(__name__)

CSV_FILE = "weather.csv"

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
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
    print("Received:", data)

    try:
        with open("weather.csv", "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow([
                datetime.now().isoformat(),
                data["temperature"],
                data["humidity"],
                data["pressure"],
                data["light"]
            ])
        print("Saved to CSV")
    except Exception as e:
        print("CSV error:", e)

    return "OK"

@app.route("/")
def index():
    return "Weather station is running."

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
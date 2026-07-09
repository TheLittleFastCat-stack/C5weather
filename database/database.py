from flask import Flask, request
import sqlite3

app = Flask(__name__)

# Create database
conn = sqlite3.connect("weather.db")
cursor = conn.cursor()

cursor.execute("""
CREATE TABLE IF NOT EXISTS weather(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    temperature REAL,
    humidity REAL,
    pressure REAL,
    light REAL
)
""")

conn.commit()
conn.close()


import csv
from datetime import datetime

@app.route("/weather", methods=["POST"])
def weather():
    data = request.get_json()

    with open("weather.csv", "a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([
            datetime.now().isoformat(),
            data["temperature"],
            data["humidity"],
            data["pressure"],
            data["light"]
        ])

    return "OK"

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
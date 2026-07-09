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


@app.route("/weather", methods=["POST"])
def weather():

    data = request.json

    conn = sqlite3.connect("weather.db")
    cursor = conn.cursor()

    cursor.execute("""
        INSERT INTO weather
        (temperature, humidity, pressure, light)
        VALUES (?,?,?,?)
    """, (
        data["temperature"],
        data["humidity"],
        data["pressure"],
        data["light"]
    ))

    conn.commit()
    conn.close()

    return "OK"


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
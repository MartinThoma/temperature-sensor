import requests
import csv
import time
from datetime import datetime

URL = "http://192.168.178.47"
CSV_FILE = "sensor_data.csv"

def get_sensor_data():
    try:
        response = requests.get(URL, timeout=5)
        response.raise_for_status()
        data = response.json()
        return {
            "timestamp": datetime.now().isoformat(timespec='seconds'),
            "temperature": data["temperature"].replace("°C", "").replace("Â", "").strip(),
            "humidity": data["humidity"].replace("%", "").strip()
        }
    except Exception as e:
        print(f"[{datetime.now().isoformat(timespec='seconds')}] Error: {e}")
        return None

def write_to_csv(data):
    file_exists = False
    try:
        file_exists = open(CSV_FILE).readline().startswith("timestamp")
    except FileNotFoundError:
        pass

    with open(CSV_FILE, mode="a", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=["timestamp", "temperature", "humidity"])
        if not file_exists:
            writer.writeheader()
        writer.writerow(data)

if __name__ == "__main__":
    print("Starting sensor data logger...")
    while True:
        sensor_data = get_sensor_data()
        if sensor_data:
            write_to_csv(sensor_data)
            print(f"Logged: {sensor_data}")
        time.sleep(60)

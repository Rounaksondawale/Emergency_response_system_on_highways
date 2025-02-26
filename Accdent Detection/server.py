import serial
import time
from datetime import datetime
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

class GPSTracker:
    def __init__(self, port='COM6', baud=9600):
        # Initialize Firebase
        cred = credentials.Certificate('credentials.json')
        firebase_admin.initialize_app(cred, {
            'databaseURL': 'https://alert-8c0c7-default-rtdb.asia-southeast1.firebasedatabase.app/'
        })
        
        # Initialize Serial
        try:
            self.ser = serial.Serial(port, baud, timeout=1)
            time.sleep(2)  # Wait for connection to establish
        except serial.SerialException as e:
            print(f"Error opening serial port: {e}")
            exit()
        
        print("GPS Tracker initialized")

    def parse_data(self, data):
        """Parse the comma-separated data from Arduino"""
        try:
            # Ignore lines that don't follow the expected format
            if not data.startswith(("NORMAL", "ACCIDENT")):
                print(f"Skipping irrelevant data: {data}")
                return None
    
            status, lat, lon, speed = data.strip().split(',')
            return {
                'status': status,
                'latitude': float(lat),
                'longitude': float(lon),
                'speed': float(speed),
                'timestamp': datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            }
        except Exception as e:
            print(f"Error parsing data: {e}")
            return None

    def upload_to_firebase(self, data):
        """Upload the GPS data to Firebase"""
        try:
            # Store in two locations:
            # 1. Latest location
            ref = db.reference('/latest_location')
            ref.set({
                'Nearest_hospital': "",
                'latitude':data['latitude'],
                'longitude':data['longitude']
            })
            
            # 2. History (only if it's an accident)
            if data['status'] == 'ACCIDENT':
                ref = db.reference('/accident_locations')
                ref.push({
                    'Nearest_hospital': "",
                    'latitude':data['latitude'],
                    'longitude':data['longitude'],
                    'Assign':False
                })
                print("Accident data uploaded to Firebase!")
            else:
                print("Location updated in Firebase")
                
        except Exception as e:
            print(f"Error uploading to Firebase: {e}")

    def run(self):
        """Main loop to read serial and upload data"""
        print("Starting GPS tracking...")

        while True:
            try:
                if self.ser.in_waiting > 0:
                    # Read raw bytes from serial
                    raw_bytes = self.ser.readline()

                    # Print raw data to check what’s being received
                    print(f"Raw Serial Bytes: {raw_bytes}")

                    # Decode, ignoring bad bytes
                    line = raw_bytes.decode('utf-8', errors='ignore').strip()
                    print(f"Decoded Serial Data: {line}")  # Debugging

                    # Parse and process the data
                    data = self.parse_data(line)
                    if data:
                        self.upload_to_firebase(data)

                        if data['status'] == 'ACCIDENT':
                            print("\n!!! ACCIDENT DETECTED !!!")
                        print(f"Location: {data['latitude']}, {data['longitude']}")
                        print(f"Speed: {data['speed']} km/h")
                        print("------------------------")

            except KeyboardInterrupt:
                print("\nProgram terminated by user")
                break
            except Exception as e:
                print(f"Error in main loop: {e}")
                continue

        self.ser.close()
        print("Serial connection closed")


if __name__ == "__main__":
    tracker = GPSTracker(port='COM6', baud=9600)
    tracker.run()
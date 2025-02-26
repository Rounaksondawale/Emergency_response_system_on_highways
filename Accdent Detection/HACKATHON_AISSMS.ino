#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Initialize sensors
Adafruit_MPU6050 mpu;
SoftwareSerial gpsSerial(3, 4); // RX, TX
TinyGPSPlus gps;

// Constants for accident detection
const float ACCEL_THRESHOLD = 1.0;
const float GYRO_THRESHOLD = 100.0;
const int LED_PIN = 13;
const int BUFFER_SIZE = 10;

// Variables for storing sensor data
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float accelMagnitude, gyroMagnitude;

// Buffers for detection
float accelBuffer[BUFFER_SIZE];
float gyroBuffer[BUFFER_SIZE];
int bufferIndex = 0;
bool bufferFilled = false;

double latitude, longitude, speed;

void setup() {
    Serial.begin(9600);
    gpsSerial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    
    while (!mpu.begin()) {
        Serial.println("MPU6050 not found!");
        delay(500);
    }

    Serial.println("MPU6050 ready!");
    delay(1000); // Let the sensor stabilize
    
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

float calculateMagnitude(float x, float y, float z) {
    return sqrt(x * x + y * y + z * z);
}

bool detectAccident() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
  
    // Get acceleration values in g's
    accelX = a.acceleration.x / 9.81;
    accelY = a.acceleration.y / 9.81;
    accelZ = a.acceleration.z / 9.81;
  
    // Get gyroscope values in degrees/second
    gyroX = g.gyro.x * RAD_TO_DEG;
    gyroY = g.gyro.y * RAD_TO_DEG;
    gyroZ = g.gyro.z * RAD_TO_DEG;
  
    // Calculate magnitudes
    accelMagnitude = calculateMagnitude(accelX, accelY, accelZ);
    gyroMagnitude = calculateMagnitude(gyroX, gyroY, gyroZ);
  
    // Update buffers
    accelBuffer[bufferIndex] = accelMagnitude;
    gyroBuffer[bufferIndex] = gyroMagnitude;
  
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
    if (bufferIndex == 0) bufferFilled = true;
  
    if (bufferFilled) {
        float accelAvg = 0;
        float gyroAvg = 0;
    
        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (i != bufferIndex) {
                accelAvg += accelBuffer[i];
                gyroAvg += gyroBuffer[i];
            }
        }
        accelAvg /= (BUFFER_SIZE - 1);
        gyroAvg /= (BUFFER_SIZE - 1);
    
        float accelChange = abs(accelMagnitude - accelAvg);
        float gyroChange = abs(gyroMagnitude - gyroAvg);
    
        if (accelChange > ACCEL_THRESHOLD && gyroChange > GYRO_THRESHOLD) {
            return true;
        }
    }
  
    return false;
}

void sendAccidentData() {
  if(gps.location.isValid()){
    Serial.print("ACCIDENT,");
    Serial.print(latitude, 6);
    Serial.print(",");
    Serial.print(longitude, 6);
    Serial.print(",");
    Serial.println(speed);
    return;
  }
  Serial.println("ACCIDENT,0.000000,0.000000,0.000000");
}

void loop() {
    bool accident = detectAccident();

    while (gpsSerial.available() > 0) {
      if(gps.encode(gpsSerial.read())){
        latitude = gps.location.lat();
    longitude = gps.location.lng();
    speed = gps.speed.kmph();
    //     Serial.print("Latitude: ");
    // Serial.print(gps.location.lat(), 6);
    // Serial.print(" Longitude: ");
    // Serial.print(gps.location.lng(), 6);
    // Serial.println();
      }
    }

    // Serial.println(gps.location.isValid());
  
    if (accident) {
        digitalWrite(LED_PIN, HIGH);
        sendAccidentData();
        delay(100);  // Send data once and wait before checking again
        digitalWrite(LED_PIN, LOW);
    }
}

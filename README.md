# Accident Detection & Emergency Response System

## Overview
This project involves a **portable IoT device** that detects vehicle accidents using **gyroscopic sensors** and an **accelerometer**. Upon detection, the system uses **GPS & Firebase** to send the accident location and automatically assigns the nearest **ambulance & hospital** using a **dynamic grid-based approach**. Additionally, two **Android apps** are developed for emergency response.

## Features
- **Accident Detection:** Monitors position and acceleration changes to identify accidents.
- **Real-time Location Sharing:** Sends accident coordinates to Firebase.
- **Grid-based Hospital Allocation:** Dynamically assigns ambulances and hospitals.
- **Android Apps:**
  - **Ambulance App** – Displays accident location and integrates **Google Maps API** for navigation.
  - **User App** – Allows manual accident reporting in case of hardware failure.

## Technologies Used
- **Hardware:** Gyroscope, Accelerometer, GPS module
- **Backend:** Firebase Realtime Database
- **Android Development:** Java/Kotlin (Android Studio)
- **Maps & Navigation:** Google Maps API

## Installation & Setup
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/accident-detection-system.git
   cd accident-detection-system
   ```
2. Open the Android apps in **Android Studio**.
3. Configure **Firebase** for real-time data synchronization.
4. Deploy the IoT device with necessary sensors and GPS module.
5. Run the apps and test the accident detection & emergency response flow.

## Usage
- Deploy the IoT device in a vehicle.
- The device will detect accidents and send location data.
- The system will assign the nearest ambulance and hospital.
- The ambulance app will guide responders using Google Maps.
- Users can manually report accidents if the hardware fails.

## Contribution
Feel free to contribute by submitting **issues** or **pull requests**. Ensure that your code follows best practices and is well-documented.

## License
This project is open-source under the **MIT License**.

---

### Contact
For any queries, contact **[Your Name]** at **your.email@example.com**.

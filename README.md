# 🌱 Smart Garden System

An Arduino-based smart irrigation system that monitors soil moisture and temperature levels to automatically water plants based on real-time data and user preferences.

## 📦 Features

- 🌡️ **Temperature Sensor** – Measures ambient temperature in the garden area.
- 💧 **Soil Moisture Sensor** – Detects current moisture level in the soil.
- 🌫️ **Humidity Sensor** – Optionally used for environmental tracking.
- 📺 **TFT Touch Display** – Interactive screen for selecting a plant and defining optimal growth parameters.
- 🚿 **Water Pump** – Activated automatically when soil is too dry for the selected plant.
- 🧠 **Plant-Specific Logic** – Each plant has its own preferred temperature, humidity, and watering level.

## 🔌 Hardware Used

| Component              | Description                        |
|------------------------|------------------------------------|
| Arduino Uno / Nano     | Microcontroller                    |
| Soil Moisture Sensor   | Analog sensor                      |
| DHT11 / LM35           | Temperature and humidity sensors   |
| TFT9341 Touch Display  | 2.4" / 2.8" touch screen           |
| Relay / MOSFET Module  | Controls the water pump            |
| Water Pump             | Small 5V-12V pump for irrigation   |
| Power Supply           | 9V battery / USB                   |

## 🧪 Results

- 🌿 System successfully detects when the soil is too dry and automatically waters it for 5 seconds.
- 🖥️ TFT screen shows selected plant, desired temperature and humidity, and watering needs.
- 📉 Prevents overwatering by allowing watering only once per selection cycle.
- 🔔 Alerts user when temperature exceeds plant-safe limits via visual warnings on the screen.

## 🚀 How It Works

1. User selects a plant on the TFT screen.
2. System retrieves the desired conditions for that plant.
3. Sensors read real-time values from the environment.
4. If soil is too dry → pump turns on for a fixed duration.
5. If temperature is too high → screen displays a warning.
6. After action, returns to the main screen and waits for next interaction.

## 🧠 Logic Flow

```
[User selects plant] → [Read sensors] → [Compare values]
→ [If moisture too low → Turn on pump]
→ [If temperature too high → Show alert]
→ [Show plant info 15 sec] → [Return to main screen]
```

## 📸 Screenshots

*Insert here photos of the TFT screen, plant selection interface, and irrigation in action.*

## 🛠️ Future Improvements

- Store plant profiles in EEPROM
- Add real-time clock for scheduled watering
- Display graphs of soil conditions over time
- Add WiFi support for mobile monitoring

---

Made with 🌱 and ❤️ by Hodaya Moyal

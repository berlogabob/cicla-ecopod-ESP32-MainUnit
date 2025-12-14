flowchart TD
    A["Start: Power On ESP32"] --> B["setup()"]
    B --> B1["Initialize Serial Monitor"]
    B1 --> B2["Connect to WiFi"]
    B2 --> B3["Setup LED PWM on GPIO25"]
    B3 --> B4["Initialize Buffer to Zero"]
    B4 --> B5["Initialize Firebase Connection"]
    B5 --> C["Enter loop() - Main Cycle"]

    C --> D{"Time to read sensors?"}
    D -->|"Yes"| E["Read 4 analog sensors"]
    E --> F["Store value + current timestamp in circular buffer"]
    F --> G["Update Buffer Index"]
    G --> H{"Time to read settings from Firebase? (every 10s)"}

    H -->|"Yes"| I["Read all settings from Firebase"]
    I --> I1["Update min_light, target_light, light_threshold"]
    I1 --> I2["Update sensor weights (0.0-1.0)"]
    I2 --> I3["Update manual_override & manual_toggle"]
    I3 --> I4["Update send_interval, qty_reading_sensors_in_second, sensor_buffer_time"]
    I4 --> I5["Recalculate read_delay"]
    I5 --> J["Calculate averages from sliding window"]

    J --> J1["For each sensor: sum values where timestamp >= now - sensor_buffer_time seconds"]
    J1 --> J2["Compute avg0, avg1, avg2, avg3"]
    J2 --> K["Calculate weighted_avg using sensor weights"]
    K --> L["Control LED"]

    L --> L1{"manual_override == 1?"}
    L1 -->|"Yes"| L2{"manual_toggle == 1?"}
    L2 -->|"Yes"| L3["brightness = 255 (full on)"]
    L2 -->|"No"| L4["brightness = 0 (off)"]
    L1 -->|"No (Auto mode)"| L5{"weighted_avg < light_threshold?"}
    L5 -->|"No"| L4
    L5 -->|"Yes"| L6["Map brightness in working range"]
    L6 --> L7["Add compensation: error = target_light - weighted_avg; brightness += error/20"]
    L7 --> L8["Clamp brightness to 0-255"]
    L8 --> L9["Set LED Brightness with ledcWrite"]

    L9 --> M{"Time to send data to Firebase?"}
    M -->|"Yes"| N["Send to Firebase"]
    N --> N1["Set avg_light = weighted_avg"]
    N1 --> N2["Set brightness = current_brightness"]
    N2 --> N3["Set timestamp"]
    N3 --> N4["Set sensors/sensors_data/sensor0 to sensor3 = avg0-avg3"]
    N4 --> C

    M -->|"No"| C
    H -->|"No"| J
    D -->|"No"| H

    style A fill:#f9f,stroke:#333
    style C fill:#bbf,stroke:#333
    style L fill:#ff9,stroke:#333

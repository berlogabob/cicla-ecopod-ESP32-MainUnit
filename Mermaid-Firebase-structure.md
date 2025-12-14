graph TD
    Devices["devices/"] --> Pod["Pod_01_base_01/"]

    Pod --> AvgLight["avg_light: 930"]
    Pod --> Brightness["brightness: 0"]
    Pod --> Timestamp["timestamp: 1736791200"]

    Pod --> Sensors["sensors/"]
    Sensors --> SensorsData["sensors_data/"]
    SensorsData --> Sensor0["sensor0: 991"]
    SensorsData --> Sensor1["sensor1: 779"]
    SensorsData --> Sensor2["sensor2: 976"]
    SensorsData --> Sensor3["sensor3: 974"]

    Pod --> Settings["settings/"]

    Settings --> LightMapping["light_mapping/"]
    LightMapping --> MinLight["min_light: 0"]
    LightMapping --> TargetLight["target_light: 1000"]
    LightMapping --> Threshold["light_threshold: 1300"]
    LightMapping --> SensorWeights["sensor_weights/"]
    SensorWeights --> W0["sensor0_weight: 1.0"]
    SensorWeights --> W1["sensor1_weight: 1.0"]
    SensorWeights --> W2["sensor2_weight: 1.0"]
    SensorWeights --> W3["sensor3_weight: 1.0"]

    Settings --> LightMode["light_mode/"]
    LightMode --> ManualOverride["manual_override: 0"]
    LightMode --> ManualToggle["manual_toggle: 0"]

    Settings --> ConnectionPackage["connection_package/"]
    ConnectionPackage --> SendInterval["send_interval: 5"]
    ConnectionPackage --> QtyReading["qty_reading_sensors_in_second: 10"]
    ConnectionPackage --> BufferTime["sensor_buffer_time: 5"]

    style Devices fill:#e3f2fd,stroke:#1976d2
    style Pod fill:#bbdefb,stroke:#1976d2
    style Sensors fill:#c8e6c9,stroke:#388e3c
    style SensorsData fill:#dcedc8,stroke:#689f38
    style Settings fill:#f3e5f5,stroke:#7b1fa2
    style LightMapping fill:#e1bee7,stroke:#512da8
    style SensorWeights fill:#d1c4e9,stroke:#4527a0
    style LightMode fill:#fff3e0,stroke:#ef6c00
    style ConnectionPackage fill:#ffe0b2,stroke:#ef6c00

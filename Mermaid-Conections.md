flowchart TD
    subgraph Power ["Power Supply"]
        P1["3.3V"]
        P2["GND"]
    end

    subgraph Sensors ["Light Sensors (Photoresistors)"]
        S0["Photoresistor 0"]
        S1["Photoresistor 1"]
        S2["Photoresistor 2"]
        S3["Photoresistor 3"]
    end

    subgraph Resistors ["Pull-down Resistors"]
        R0["10kΩ"]
        R1["10kΩ"]
        R2["10kΩ"]
        R3["10kΩ"]
    end

    subgraph ESP32 ["ESP32 Pins"]
        GPIO34["GPIO34"]
        GPIO35["GPIO35"]
        GPIO32["GPIO32"]
        GPIO33["GPIO33"]
        GPIO25["GPIO25 (PWM)"]
    end

    subgraph LED_Part ["LED"]
        LED["LED"]
        RL["220-330Ω Resistor"]
    end

    P1 --> S0
    P1 --> S1
    P1 --> S2
    P1 --> S3

    S0 --> GPIO34
    S1 --> GPIO35
    S2 --> GPIO32
    S3 --> GPIO33

    GPIO34 --> R0
    GPIO35 --> R1
    GPIO32 --> R2
    GPIO33 --> R3

    R0 --> P2
    R1 --> P2
    R2 --> P2
    R3 --> P2

    GPIO25 --> LED
    LED --> RL
    RL --> P2

    style Power fill:#e1f5fe
    style Sensors fill:#f0fff0
    style Resistors fill:#fff3e0
    style ESP32 fill:#fff0f5
    style LED_Part fill:#ffebee
    style P1 fill:#bbdefb
    style P2 fill:#bbdefb

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// WiFi
const char* ssid = "MEO-2hzF96460";
const char* password = "FpxA9bv8";

// Firebase
#define FIREBASE_HOST "booking-ee47f-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "m3uCFaiui2EXuQdpZGuuIgwgarKXH5lojbhUgF5b"

// Имя устройства
const String device_id = "Pod_01_base_01";

// Пины
const int sensorPin0 = 34;
const int sensorPin1 = 35;
const int sensorPin2 = 32;
const int sensorPin3 = 33;
const int ledPin = 25;

// Буфер (фиксированный размер 10)
const int buffer_size = 10;
int buffer0[buffer_size];
int buffer1[buffer_size];
int buffer2[buffer_size];
int buffer3[buffer_size];
int buffer_index = 0;

// Настройки (начальные значения)
int min_light = 0;
int max_light = 4095;
int target_light = 1000;
int light_threshold = 1300;
int send_interval = 5;                  // секунды между пакетами
int qty_reading_sensors_in_second = 10; // раз в секунду (10 = delay 100ms)

// Текущая яркость
int current_brightness = 0;

// Объекты Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  ledcAttach(ledPin, 5000, 8);

  // Буфер нулями
  int i;
  for (i = 0; i < buffer_size; i++) {
    buffer0[i] = 0;
    buffer1[i] = 0;
    buffer2[i] = 0;
    buffer3[i] = 0;
  }

  // Firebase
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Ready");
}

void loop() {
  // Чтение датчиков
  buffer0[buffer_index] = analogRead(sensorPin0);
  buffer1[buffer_index] = analogRead(sensorPin1);
  buffer2[buffer_index] = analogRead(sensorPin2);
  buffer3[buffer_index] = analogRead(sensorPin3);

  buffer_index = buffer_index + 1;
  if (buffer_index >= buffer_size) {
    buffer_index = 0;
  }

  // Средние
  int sum0 = 0;
  int sum1 = 0;
  int sum2 = 0;
  int sum3 = 0;
  int j;
  for (j = 0; j < buffer_size; j++) {
    sum0 = sum0 + buffer0[j];
    sum1 = sum1 + buffer1[j];
    sum2 = sum2 + buffer2[j];
    sum3 = sum3 + buffer3[j];
  }
  int avg0 = sum0 / buffer_size;
  int avg1 = sum1 / buffer_size;
  int avg2 = sum2 / buffer_size;
  int avg3 = sum3 / buffer_size;
  int avg = (avg0 + avg1 + avg2 + avg3) / 4;

  // === Чтение настроек каждые 10 сек ===
  static unsigned long last_read = 0;
  if (millis() - last_read > 10000) {
    String settings_path = "devices/" + device_id + "/settings/light_mapping/";
    if (Firebase.RTDB.getInt(&fbdo, settings_path + "min_light")) min_light = fbdo.intData();
    if (Firebase.RTDB.getInt(&fbdo, settings_path + "max_light")) max_light = fbdo.intData();
    if (Firebase.RTDB.getInt(&fbdo, settings_path + "target_light")) target_light = fbdo.intData();
    if (Firebase.RTDB.getInt(&fbdo, settings_path + "light_threshold")) light_threshold = fbdo.intData();

    String conn_path = "devices/" + device_id + "/settings/connection_package/";
    if (Firebase.RTDB.getInt(&fbdo, conn_path + "send_interval")) send_interval = fbdo.intData();
    if (Firebase.RTDB.getInt(&fbdo, conn_path + "qty_reading_sensors_in_second")) {
      qty_reading_sensors_in_second = fbdo.intData();
      if (qty_reading_sensors_in_second < 1) qty_reading_sensors_in_second = 1;
    }

    last_read = millis();
    Serial.println("Settings updated from Firebase");
  }

  // === Управление LED ===
  int brightness = 0;
  if (avg < light_threshold) {  // Темно
    brightness = (max_light - avg) * 255 / (max_light - min_light);
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;

    // Компенсация к target
    int error = target_light - avg;
    brightness = brightness + error / 10;
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
  }
  ledcWrite(ledPin, brightness);
  current_brightness = brightness;

  // === Отправка пакета ===
  static unsigned long last_send = 0;
  unsigned long now = millis();
  if (now - last_send > (send_interval * 1000)) {
    String base_path = "devices/" + device_id + "/";

    Firebase.RTDB.setInt(&fbdo, base_path + "avg_light", avg);
    Firebase.RTDB.setInt(&fbdo, base_path + "brightness", current_brightness);
    Firebase.RTDB.setInt(&fbdo, base_path + "timestamp", now / 1000);

    String sensors_path = base_path + "sensors/";
    Firebase.RTDB.setInt(&fbdo, sensors_path + "sensor0", avg0);
    Firebase.RTDB.setInt(&fbdo, sensors_path + "sensor1", avg1);
    Firebase.RTDB.setInt(&fbdo, sensors_path + "sensor2", avg2);
    Firebase.RTDB.setInt(&fbdo, sensors_path + "sensor3", avg3);

    last_send = now;
  }

  // Задержка для частоты чтения датчиков
  int read_delay = 1000 / qty_reading_sensors_in_second;  // ms
  if (read_delay < 10) read_delay = 10;  // минимум
  delay(read_delay);
}
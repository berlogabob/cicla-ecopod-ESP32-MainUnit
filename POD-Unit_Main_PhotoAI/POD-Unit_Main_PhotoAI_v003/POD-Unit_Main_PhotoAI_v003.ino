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

// Буфер
const int buffer_size = 10;
int buffer0[buffer_size];
int buffer1[buffer_size];
int buffer2[buffer_size];
int buffer3[buffer_size];
int buffer_index = 0;

// Настройки из Firebase
int light_threshold = 1300;  // Новый порог по умолчанию
int send_interval = 5;       // Секунд
int min_light = 0;           // Мин свет для маппинга
int max_light = 4095;        // Макс свет
int target_light = 1000;     // Цель для компенсации
int current_brightness = 0;  // Текущая яркость для компенсации

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
  for (int i = 0; i < buffer_size; i++) {
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

  buffer_index++;
  if (buffer_index >= buffer_size) buffer_index = 0;

  // Средние
  int sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
  for (int j = 0; j < buffer_size; j++) {
    sum0 += buffer0[j];
    sum1 += buffer1[j];
    sum2 += buffer2[j];
    sum3 += buffer3[j];
  }
  int avg0 = sum0 / buffer_size;
  int avg1 = sum1 / buffer_size;
  int avg2 = sum2 / buffer_size;
  int avg3 = sum3 / buffer_size;
  int avg = (avg0 + avg1 + avg2 + avg3) / 4;

  // === Чтение настроек каждые 10 сек ===
  static unsigned long last_read = 0;
  if (millis() - last_read > 10000) {
    String path = "devices/" + device_id + "/";
    if (Firebase.RTDB.getInt(&fbdo, path + "light_threshold")) light_threshold = fbdo.intData();
    if (Firebase.RTDB.getInt(&fbdo, path + "send_interval")) send_interval = fbdo.intData();
    if (Firebase.RTDB.getInt(&fbdo, path + "min_light")) min_light = fbdo.intData();
    if (Firebase.RTDB.getInt(&fbdo, path + "max_light")) max_light = fbdo.intData();
    if (Firebase.RTDB.getInt(&fbdo, path + "target_light")) target_light = fbdo.intData();
    last_read = millis();
  }

  // === Порог + маппинг + компенсация ===
  int brightness = 0;
  if (avg < light_threshold) {  // Темно — начать компенсацию
    // Плавное маппинг: от min к max -> 255 к 0
    brightness = (max_light - avg) * 255 / (max_light - min_light);
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;

    // Авто-компенсация к target
    int error = target_light - avg;  // Разница
    brightness = brightness + error / 10;  // Корректировка (делим на 10 для медленности)
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
  }
  ledcWrite(ledPin, brightness);
  current_brightness = brightness;  // Сохраняем для отправки

  // === Отправка ===
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

  delay(100);
}
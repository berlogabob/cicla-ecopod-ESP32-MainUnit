#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Твои настройки WiFi
const char* ssid = "MEO-2hzF96460";
const char* password = "FpxA9bv8";

// Настройки Firebase
#define FIREBASE_HOST "booking-ee47f-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "m3uCFaiui2EXuQdpZGuuIgwgarKXH5lojbhUgF5b"

// Имя устройства
const String device_id = "Pod_01_base_01";

// Пины датчиков
const int sensorPin0 = 34;
const int sensorPin1 = 35;
const int sensorPin2 = 32;
const int sensorPin3 = 33;

// Пин LED
const int ledPin = 25;

// Буфер для среднего (10 показаний)
const int buffer_size = 10;
int buffer0[buffer_size];
int buffer1[buffer_size];
int buffer2[buffer_size];
int buffer3[buffer_size];
int buffer_index = 0;

// Объект Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  // Подключение WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Настройка PWM для LED (новый способ)
  ledcAttach(ledPin, 5000, 8);  // пин, частота 5kHz, 8-бит

  // Заполнение буфера нулями
  int i;
  for (i = 0; i < buffer_size; i++) {
    buffer0[i] = 0;
    buffer1[i] = 0;
    buffer2[i] = 0;
    buffer3[i] = 0;
  }

  // Настройка Firebase
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase ready");
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

  // Средние значения
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

  // Яркость LED (темнее — ярче)
  int brightness = (4095 - avg) * 255 / 4095;
  ledcWrite(ledPin, brightness);

  // Отправка каждые 5 секунд
  static unsigned long last_send = 0;
  unsigned long now = millis();
  if (now - last_send > 5000) {
    String base_path = "devices/" + device_id + "/";

    if (Firebase.RTDB.setInt(&fbdo, base_path + "avg_light", avg)) {
      Serial.println("avg_light sent");
    }
    if (Firebase.RTDB.setInt(&fbdo, base_path + "brightness", brightness)) {
      Serial.println("brightness sent");
    }
    if (Firebase.RTDB.setInt(&fbdo, base_path + "timestamp", now / 1000)) {
      Serial.println("timestamp sent");
    }

    String sensors_path = base_path + "sensors/";
    Firebase.RTDB.setInt(&fbdo, sensors_path + "sensor0", avg0);
    Firebase.RTDB.setInt(&fbdo, sensors_path + "sensor1", avg1);
    Firebase.RTDB.setInt(&fbdo, sensors_path + "sensor2", avg2);
    Firebase.RTDB.setInt(&fbdo, sensors_path + "sensor3", avg3);

    last_send = now;
    Serial.println("All data sent");
  }

  delay(100);
}
#include <WiFi.h>
#include <FirebaseESP32.h>

// Настройки WiFi
const char* ssid = "MEO-2hzF96460";
const char* password = "FpxA9bv8";

// Настройки Firebase
#define FIREBASE_HOST "booking-ee47f-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "m3uCFaiui2EXuQdpZGuuIgwgarKXH5lojbhUgF5b"

// Уникальное имя устройства
const String device_id = "Pod_01_base_01";

// Пины датчиков (4 датчика света)
const int sensorPin0 = 34;
const int sensorPin1 = 35;
const int sensorPin2 = 32;
const int sensorPin3 = 33;

// Пин LED (PWM)
const int ledPin = 25;

// Буфер для среднего (4 датчика, 10 показаний каждый)
const int buffer_size = 10;
int buffer0[buffer_size];
int buffer1[buffer_size];
int buffer2[buffer_size];
int buffer3[buffer_size];
int buffer_index = 0;

// Объект Firebase
FirebaseData firebase;

void setup() {
  // Запуск serial для отладки
  Serial.begin(115200);
  
  // Подключение к WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  
  // Запуск Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  // Установка пинов датчиков как вход
  pinMode(sensorPin0, INPUT);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);
  
  // Установка пина LED как PWM
  ledcSetup(0, 5000, 8);  // Канал 0, 5kHz, 8-бит
  ledcAttachPin(ledPin, 0);
  
  // Заполнение буфера нулями
  int i;
  for (i = 0; i < buffer_size; i++) {
    buffer0[i] = 0;
    buffer1[i] = 0;
    buffer2[i] = 0;
    buffer3[i] = 0;
  }
}

void loop() {
  // Чтение датчиков и обновление буфера
  buffer0[buffer_index] = analogRead(sensorPin0);
  buffer1[buffer_index] = analogRead(sensorPin1);
  buffer2[buffer_index] = analogRead(sensorPin2);
  buffer3[buffer_index] = analogRead(sensorPin3);
  
  // Перемещение индекса
  buffer_index = buffer_index + 1;
  if (buffer_index >= buffer_size) {
    buffer_index = 0;
  }
  
  // Вычисление среднего для каждого датчика
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
  
  int sensor_avg_0 = sum0 / buffer_size;
  int sensor_avg_1 = sum1 / buffer_size;
  int sensor_avg_2 = sum2 / buffer_size;
  int sensor_avg_3 = sum3 / buffer_size;
  
  // Общее среднее
  int avg = (sensor_avg_0 + sensor_avg_1 + sensor_avg_2 + sensor_avg_3) / 4;
  
  // Установка яркости (просто: больше света = меньше LED)
  int brightness = 0;
  if (avg <= 4095) {
    brightness = (4095 - avg) * 255 / 4095;
  }
  ledcWrite(0, brightness);
  
  // Отправка в Firebase каждые 5 секунд
  static unsigned long last_send = 0;
  unsigned long now = millis();
  if (now - last_send > 5000) {
    String base_path = "devices/" + device_id + "/";
    
    Firebase.setFloat(firebase, base_path + "avg_light", avg);
    Firebase.setInt(firebase, base_path + "brightness", brightness);
    Firebase.setLong(firebase, base_path + "timestamp", now / 1000);
    
    String sensor_path = base_path + "sensors/";
    Firebase.setInt(firebase, sensor_path + "sensor0", sensor_avg_0);
    Firebase.setInt(firebase, sensor_path + "sensor1", sensor_avg_1);
    Firebase.setInt(firebase, sensor_path + "sensor2", sensor_avg_2);
    Firebase.setInt(firebase, sensor_path + "sensor3", sensor_avg_3);
    
    last_send = now;
  }
  
  // Маленькая задержка
  delay(100);
}
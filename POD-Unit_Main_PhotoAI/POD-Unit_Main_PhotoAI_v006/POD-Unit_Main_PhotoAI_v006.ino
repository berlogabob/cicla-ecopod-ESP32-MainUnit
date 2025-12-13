#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// WiFi
//const char* ssid = "MEO-2hzF96460";
//const char* password = "FpxA9bv8";

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

// Буфер (максимум 600 показаний)
const int max_buffer_size = 600;
int buffer0[max_buffer_size];
int buffer1[max_buffer_size];
int buffer2[max_buffer_size];
int buffer3[max_buffer_size];
int buffer_index = 0;

// Настройки (начальные)
int min_light = 0;
int target_light = 1000;
int light_threshold = 1300;
int send_interval = 5;
int qty_reading_sensors_in_second = 10;
int sensor_buffer_time = 5;
int manual_override = 0;   // 0 = авто, 1 = ручной
int manual_toggle = 0;     // 0 = выкл, 1 = вкл (ручной)

// Веса датчиков 0.0 - 1.0 (начальные = 1.0)
float sensor0_weight = 1.0;
float sensor1_weight = 1.0;
float sensor2_weight = 1.0;
float sensor3_weight = 1.0;

// Вычисляемый размер буфера
int current_buffer_size = 50;

// Таймеры (неблокирующий код)
unsigned long last_sensor_read = 0;
unsigned long last_settings_read = 0;
unsigned long last_send = 0;
int read_delay = 100;  // ms

// Яркость
int current_brightness = 0;

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  ledcAttach(ledPin, 5000, 8);

  // Буфер нулями
  int i;
  for (i = 0; i < max_buffer_size; i++) {
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

  Serial.println("Ready - non-blocking");
}

void loop() {
  unsigned long now = millis();

  // Чтение датчиков по таймеру
  if (now - last_sensor_read >= read_delay) {
    buffer0[buffer_index] = analogRead(sensorPin0);
    buffer1[buffer_index] = analogRead(sensorPin1);
    buffer2[buffer_index] = analogRead(sensorPin2);
    buffer3[buffer_index] = analogRead(sensorPin3);

    buffer_index = buffer_index + 1;
    if (buffer_index >= current_buffer_size) {
      buffer_index = 0;
    }

    last_sensor_read = now;
  }

  // Чтение настроек каждые 10 сек
  if (now - last_settings_read >= 10000) {
    String light_path = "devices/" + device_id + "/settings/light_mapping/";
    String conn_path = "devices/" + device_id + "/settings/connection_package/";
    String mode_path = "devices/" + device_id + "/settings/light_mode/";
    String weights_path = light_path + "sensor_weights/";

    Firebase.RTDB.getInt(&fbdo, light_path + "min_light", &min_light);
    Firebase.RTDB.getInt(&fbdo, light_path + "target_light", &target_light);
    Firebase.RTDB.getInt(&fbdo, light_path + "light_threshold", &light_threshold);

    // Веса датчиков
    Firebase.RTDB.getFloat(&fbdo, weights_path + "sensor0_weight", &sensor0_weight);
    Firebase.RTDB.getFloat(&fbdo, weights_path + "sensor1_weight", &sensor1_weight);
    Firebase.RTDB.getFloat(&fbdo, weights_path + "sensor2_weight", &sensor2_weight);
    Firebase.RTDB.getFloat(&fbdo, weights_path + "sensor3_weight", &sensor3_weight);

    Firebase.RTDB.getInt(&fbdo, conn_path + "send_interval", &send_interval);
    Firebase.RTDB.getInt(&fbdo, conn_path + "qty_reading_sensors_in_second", &qty_reading_sensors_in_second);
    Firebase.RTDB.getInt(&fbdo, conn_path + "sensor_buffer_time", &sensor_buffer_time);

    Firebase.RTDB.getInt(&fbdo, mode_path + "manual_override", &manual_override);
    Firebase.RTDB.getInt(&fbdo, mode_path + "manual_toggle", &manual_toggle);

    // Пересчёт размера буфера
    current_buffer_size = sensor_buffer_time * qty_reading_sensors_in_second;
    if (current_buffer_size < 1) current_buffer_size = 1;
    if (current_buffer_size > max_buffer_size) current_buffer_size = max_buffer_size;

    read_delay = 1000 / qty_reading_sensors_in_second;
    if (read_delay < 10) read_delay = 10;

    last_settings_read = now;
  }

  // Расчёт среднего (взвешенное)
  int sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
  int j;
  for (j = 0; j < current_buffer_size; j++) {
    sum0 += buffer0[j];
    sum1 += buffer1[j];
    sum2 += buffer2[j];
    sum3 += buffer3[j];
  }
  int avg0 = sum0 / current_buffer_size;
  int avg1 = sum1 / current_buffer_size;
  int avg2 = sum2 / current_buffer_size;
  int avg3 = sum3 / current_buffer_size;

  float total_weight = sensor0_weight + sensor1_weight + sensor2_weight + sensor3_weight;
  if (total_weight == 0) total_weight = 1;
  int weighted_avg = (avg0 * sensor0_weight + avg1 * sensor1_weight + avg2 * sensor2_weight + avg3 * sensor3_weight) / total_weight;

  // Управление LED
  int brightness = 0;
  if (manual_override == 1) {
    if (manual_toggle == 1) {
      brightness = 255;
    } else {
      brightness = 0;
    }
  } else {
    if (weighted_avg < light_threshold) {
      brightness = (light_threshold - weighted_avg) * 255 / (light_threshold - min_light);
      if (brightness < 0) brightness = 0;
      if (brightness > 255) brightness = 255;

      int error = target_light - weighted_avg;
      brightness = brightness + error / 20;
      if (brightness < 0) brightness = 0;
      if (brightness > 255) brightness = 255;
    }
  }
  ledcWrite(ledPin, brightness);
  current_brightness = brightness;

  // Отправка данных (в sensors/sensors_data/)
  if (now - last_send >= (send_interval * 1000)) {
    String base_path = "devices/" + device_id + "/";

    Firebase.RTDB.setInt(&fbdo, base_path + "avg_light", weighted_avg);
    Firebase.RTDB.setInt(&fbdo, base_path + "brightness", current_brightness);
    Firebase.RTDB.setInt(&fbdo, base_path + "timestamp", now / 1000);

    String sensors_data_path = base_path + "sensors/sensors_data/";  // как ты хотел
    Firebase.RTDB.setInt(&fbdo, sensors_data_path + "sensor0", avg0);
    Firebase.RTDB.setInt(&fbdo, sensors_data_path + "sensor1", avg1);
    Firebase.RTDB.setInt(&fbdo, sensors_data_path + "sensor2", avg2);
    Firebase.RTDB.setInt(&fbdo, sensors_data_path + "sensor3", avg3);

    last_send = now;
  }

  // Минимальная пауза
  delayMicroseconds(1000);  // 1 мс
}
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define FONA_RX            2
#define FONA_TX            3
#define FONA_RST           4

#define FONA_RI_INTERRUPT  0

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

char PHONE_1[21] = "+919585347608"; // Enter your Number here.

int gas_sensor_pin_MQ4 = A0;   // Analog pin for MQ-4 sensor
int gas_sensor_pin_MQ7 = A1;   // Analog pin for MQ-7 sensor
int gas_sensor_pin_MQ135 = A2; // Analog pin for MQ-135 sensor

int gas_threshold_MQ4 = 530;   // Threshold for MQ-4 sensor
int gas_threshold_MQ7 = 250;   // Threshold for MQ-7 sensor

char gasalert_MQ4[50] = "MQ-4 Gas Leakage Detected";
char gasalert_MQ7[50] = "MQ-7 Gas Leakage Detected";
char gasalert_MQ135[50] = "MQ-135 Gas Leakage Detected";

void setup() {
  pinMode(gas_sensor_pin_MQ4, INPUT);
  pinMode(gas_sensor_pin_MQ7, INPUT);
  pinMode(gas_sensor_pin_MQ135, INPUT);
  
  Serial.begin(115200);
  Serial.println(F("Initializing....(May take 3 seconds)"));
  delay(5000);
  fonaSS.begin(9600);
  if (!fona.begin(fonaSS)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }

  fona.print("AT+CSMP=17,167,0,0\r");
  Serial.println(F("FONA is OK"));
}

void loop() {
  int gas_value_MQ4 = analogRead(gas_sensor_pin_MQ4);
  int gas_value_MQ7 = analogRead(gas_sensor_pin_MQ7);
  int gas_value_MQ135 = analogRead(gas_sensor_pin_MQ135);

  checkAndSendGasAlert(gas_value_MQ4, gas_threshold_MQ4, gasalert_MQ4);
  checkAndSendGasAlert(gas_value_MQ7, gas_threshold_MQ7, gasalert_MQ7);
  checkAndSendGasAlert(gas_value_MQ135, gas_threshold_MQ4, gasalert_MQ135);
}

void checkAndSendGasAlert(int gas_value, int threshold, char gasalert[]) {
  Serial.print("GasValue: ");
  Serial.println(gas_value);

  if (gas_value > threshold) {
    Serial.println(gasalert);
    make_multi_call();
    send_multi_sms();
  }
}

void send_multi_sms() {
  if (PHONE_1 != "") {
    Serial.print("Phone 1: ");
    fona.sendSMS(PHONE_1, gasalert_MQ4); // You can change the alert message based on the sensor
    delay(10000);
  }
}

void make_multi_call() {
  if (PHONE_1 != "") {
    Serial.print("Phone 1: ");
    make_call(PHONE_1);
    delay(20000);
  }
}

void make_call(String phone) {
  Serial.println("calling....");
  fona.println("ATD" + phone + ";");
  delay(20000);
  fona.println("ATH");
  delay(1000);
}

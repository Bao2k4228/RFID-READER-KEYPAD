#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_G 4 // Định nghĩa chân LED xanh qua một transistor
#define LED_R 5 // Định nghĩa chân LED đỏ qua một transistor
#define BUZZER 2 // Chân của Buzzer
#define lock 3 // Chân điều khiển khóa

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Tạo phiên bản MFRC522
int Btn = 6; // Chân nút bấm

// Cài đặt bàn phím
const byte ROWS = 4; // Bốn hàng
const byte COLS = 4; // Bốn cột
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A3, A2, A1, A0}; // kết nối hàng của bàn phím với A3, A2, A1, A0
byte colPins[COLS] = {A7, A6, A5, A4}; // kết nối cột của bàn phím với A7, A6, A5, A4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() 
{
  Serial.begin(9600);   // Khởi tạo giao tiếp Serial
  SPI.begin();      // Khởi tạo bus SPI
  mfrc522.PCD_Init();   // Khởi tạo MFRC522
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  pinMode(Btn, INPUT);
  pinMode(lock, OUTPUT);
  Serial.println("Place your card on reader...");
  Serial.println();
}

void loop() 
{
  char key = keypad.getKey();
  
  if (key){
    Serial.print("Key pressed: ");
    Serial.println(key);
    if (key == 'A') {  
      Serial.println("Access Granted via Keypad");
      grantAccess();
    }
  }

  // Kiểm tra thẻ mới
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Chọn một trong các thẻ
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Hiển thị UID trên Serial Monitor
  Serial.print("UID tag: ");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message: ");
  content.toUpperCase();

  if (content.substring(1) == "6D DF AE 21") { // Thay đổi tại đây với UID của thẻ/thẻ mà bạn muốn cấp quyền truy cập
    grantAccess();
  } else {
    denyAccess();
  }
}

void grantAccess() {
  Serial.println("Access Granted");
  delay(500);
  digitalWrite(LED_G, HIGH); // Bật LED xanh
  
  tone(BUZZER, 2000);
  delay(100);
  noTone(BUZZER);
  delay(50);
  tone(BUZZER, 2000);
  delay(100);
  noTone(BUZZER);
  digitalWrite(lock, HIGH); // Mở khóa
  delay(3000); 
  digitalWrite(lock, LOW);  // Đóng khóa
  delay(100);
  digitalWrite(LED_G, LOW); // Tắt LED xanh
  
  tone(BUZZER, 2000);
  delay(100);
  noTone(BUZZER);
  delay(50);
}

void denyAccess() {
  Serial.println("Access Denied");
  digitalWrite(LED_R, HIGH); // Bật LED đỏ
  tone(BUZZER, 1500);
  delay(500);
  digitalWrite(LED_R, LOW); // Tắt LED đỏ
  noTone(BUZZER);
  delay(100);
  digitalWrite(LED_R, HIGH); // Bật LED đỏ
  tone(BUZZER, 1500);
  delay(500);
  digitalWrite(LED_R, LOW); // Tắt LED đỏ
  noTone(BUZZER);
  delay(100);
  digitalWrite(LED_R, HIGH); // Bật LED đỏ
  tone(BUZZER, 1500);
  delay(500);
  digitalWrite(LED_R, LOW); // Tắt LED đỏ
  noTone(BUZZER);
}

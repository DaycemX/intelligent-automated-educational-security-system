#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <Servo.h>
// Define student records
String names[] = {"Khalil", "Islem", "Daycem", "Mokhtar"};
char attendance[] = {'A', 'A', 'A', 'A'}; // 'A' for absent, 'P' for present
int state=0;
const int sensor=700 ,n;
// Pin Definitions
#define buzzer 8
#define gazeD A0 // Gas detection sensor
#define Professor 3
#define Student_1 4
#define Student_2 5
#define Student_3 6
#define Student_4 7
#define button_OFF 2
#define ldrLeft A2
#define ldrRight A1
#define tempPin A3
#define motorPin1 10
#define motorPin2 12
Servo serv,servo;
// Objet LCD avec adresse I2C 0x27 pour un écran 16x2
Adafruit_LiquidCrystal lcd(0);
void setup() {
  // Attach servo and set up pins
  serv.attach(9);
  servo.attach(11);
  servo.write(90); // Attacher le servomoteur à la broche
  // Positionner le servo au centre
  for(int i=4;i<=8;i++){
  pinMode(i,OUTPUT);
  }
  pinMode(Professor, INPUT);
  pinMode(gazeD, INPUT);
  pinMode(button_OFF, INPUT);
  // Configuration des broches
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  // Initialisation de l'écran LCD
  lcd.begin(16, 2);  // Initialize 16x2 LCD
  lcd.setBacklight(1); // Turn on backlight
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
  Serial.begin(9600);
}

void loop() {
    serv.write(0); // Close door
    // Lecture 
    int sensorValue = analogRead(tempPin);
    float voltage = sensorValue * (5.0 / 1023.0); // Conversion en tension
    float temperatureC = (voltage * 100.0)-50.8;        // Conversion en °C
    int professor = digitalRead(Professor);
    int buttonOFF = digitalRead(button_OFF);
    int gasDetected = analogRead(gazeD);
    int leftValue = analogRead(ldrLeft);  // Lecture de la LDR gauche
    int rightValue = analogRead(ldrRight); // Lecture de la LDR droite
    int tolerance= abs (rightValue-leftValue)
    n = map(tolerance,28,929,0,180);
    if (gasDetected >sensor ) {
    // Gas detected, activate buzzer and alert
    lcd.clear(); // Nettoyage de l'écran avant mise à jour
    lcd.setCursor(0, 0);
    lcd.print("Gas Detected! Alert! ");
    delay(1000);
    Serial.println("Gas Detected! Alert!");
    tone(buzzer,1000,1000);
    serv.write(0);
  }
  
    //Le professor
    if (professor == HIGH ) {
    lcd.clear(); // Nettoyage de l'écran avant mise à jour
    lcd.setCursor(0, 0);
    lcd.print("SYSTEM ON ");
    serv.write(90); // Open door
    delay(1000);    // Keep the door open for 5 seconds
     state=1;
    }
    if (buttonOFF==1){
    state=0;
    lcd.clear(); // Nettoyage de l'écran avant mise à jour
    lcd.setCursor(0, 0);
    lcd.print("SYSTEM OFF");
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    serv.write(0);
      delay(50);
  }
  
    // Contrôle du moteur en fonction de la température
    if (temperatureC > 25 && state==1 ) {
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Climatisation ON ");
      lcd.setCursor(0, 0);
      lcd.print("Temp  : ");
      lcd.print(temperatureC, 0);
      delay(1000);
    } else if (temperatureC < 18 && state==1){
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("  Chauffage ON  ");
      lcd.setCursor(0, 0);
      lcd.print("Temp  : ");
      lcd.print(temperatureC, 0);
      delay(1000);
    }
  else {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  }
  
  // Read Professor presence
  // Read attendance for students
  int studentPins[] = {Student_1, Student_2, Student_3, Student_4};
  for (int i = 0; i < 4; i++) {
    if (digitalRead(studentPins[i]) == HIGH && state==1) {
      attendance[i] = 'P'; // Mark as present
    }
  }
  // Open door if professor is present
  // Output attendance to Serial Monitor
  Serial.println("Liste De Présence :");
  for (int i = 0; i < 4; i++) {
    Serial.print(names[i]);
    Serial.print(": ");
    Serial.println(attendance[i]);
  }
  if (buttonOFF==1){
    state=0;
    lcd.clear(); // Nettoyage de l'écran avant mise à jour
    lcd.setCursor(0, 0);
    lcd.print("SYSTEM OFF");
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    serv.write(0);
  }
  // Comparaison des valeurs des LDRs

  if (leftValue > rightValue) {
    servo.write(180);  // Rotate to the left
  } else if (rightValue > leftValue) {
    servo.write(n); // Rotate to the right
  } else {
    servo.write(90);  // Neutral position if values are close
  }

  Serial.println(temperatureC);
  delay(50); // Pause pour stabiliser la boucle
  
 }
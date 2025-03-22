#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

Servo myServo;

const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {7, 6, 5, 4};  
byte colPins[COLS] = {A0, A1, A2, A3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const char correctPassword[] = "1234";
char enteredPassword[5] = "";
int index = 0;
int wrongAttempts = 0;  
bool locked = false;    
unsigned long lockStartTime = 0;

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("Enter Password:");
  myServo.attach(3);  
  myServo.write(180);    
}

void loop() {
  if (locked) {
    unsigned long elapsed = (millis() - lockStartTime) / 1000; 
    int remainingTime = 60 - elapsed; 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Locked Wait for :");
    lcd.setCursor(0, 1);
    lcd.print(remainingTime);
    lcd.print(" sec ");

    if (remainingTime <= 0) {  
      locked = false;
      wrongAttempts = 0;
      lcd.clear();
      lcd.print("Enter Password:");
    }
    delay(1000); 
    return;
  }

  char key = keypad.getKey();
  
  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    if (key == '*') {  
      index = 0;
      lcd.clear();
      lcd.print("Enter Password:");
    } 
    else if (key == '#' && index == 4) {  
      enteredPassword[index] = '\0';
      lcd.clear();

      if (strcmp(enteredPassword, correctPassword) == 0) {
        lcd.print("Access Granted");
        Serial.println("Access Granted");
        wrongAttempts = 0; 
        
        
        myServo.write(0);
        delay(2000);
        myServo.write(180);

      } else {
        lcd.print("Wrong Password");
        Serial.println("Wrong Password");
        wrongAttempts++;

        if (wrongAttempts >= 3) { 
          locked = true;
          lockStartTime = millis();
        }
      }

      delay(2000);
      lcd.clear();
      lcd.print("Enter Password:");
      index = 0;
    } 
    else if (key == 'B' && index > 0) {  
      index--;
      lcd.setCursor(index, 1);
      lcd.print(' ');  
      lcd.setCursor(index, 1);
    }
    else if (index < 4 && key != '#' && key != 'B') {
      enteredPassword[index] = key;
      index++;
      lcd.setCursor(index - 1, 1);
      lcd.print('*');
    }
  }
}

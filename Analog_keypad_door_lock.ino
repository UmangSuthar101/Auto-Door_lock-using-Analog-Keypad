#include<LiquidCrystal.h>
#include <Servo.h>
//#include <Keypad.h>
#define NO_KEY 0
#include<EEPROM.h>
int buzzer = 10;
#define greenLED 11
#define blueLED 12
#define NightLED 2
extern volatile unsigned long timer0_millis;
#define SECS_PER_MIN  (60UL)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
int elapsed_time_s = 0;
int set_time_s = 10;
int mode = 0;
char p[6] = {'0', '0', '0', '0', '0', '4'};
String stringOne;
char password[6];
char pass[6], pass1[6];
int i = 0; //keypad position
char key = 0;
const byte ROWS = 4; //number of the keypad's rows and columns
const byte COLS = 4;
Servo ServoMotor;
char keys[ROWS][COLS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};


//byte rowPins[ROWS] = {A0, A1, A2, A3}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {A4, A5, 3}; //connect to the column pinouts of the keypad

//Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int keyPadAnalogPin = 0;
unsigned int keyVal1 = 0;
unsigned int keyVal2 = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  ServoMotor.attach(13);
  ServoMotor.write(90);
  pinMode(buzzer, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(NightLED, OUTPUT);
  setLocked (true); //state of the password
  lcd.print("    Welcome ");
  Serial.println("Welcome");
  lcd.setCursor(0, 1);
  delay(3000);
  lcd.clear();
  lcd.print("Enter Password:");
  Serial.print("Enter Password:");
  lcd.setCursor(0, 1);

  for (int j = 0; j < 6; j++) {
    pass[j] = EEPROM.read(j);
   // Serial.println(pass[j]);
  }
  for (int j = 0; j < 6; j++) {
    pass[j] = EEPROM.read(j);
   // Serial.print(pass[j]);
  }
  if (pass[0] == '0' || pass[0] == '1' || pass[0] == '2' || pass[0] == '3' || pass[0] == '4' || pass[0] == '5' || pass[0] == '6' || pass[0] == '7' || pass[0] == '8' || pass[0] == '9' || pass[0] == '#') {
  //  Serial.println("success");
  }
  else {
    Serial.println("No success");
    for (int j = 0; j < 6; j++) {
      EEPROM.write(j, j + 49);
    }
  }
}

void loop()
{
  ServoMotor.write(90);
  char key = getKey();
  if (key == '*') {
    timer0_millis = 0;
    mode = 1;
  }
  if (key == '#') {
    change();
  }
  if (key)
  {
    password[i++] = key;
    beep();
    if (key == '*') {
      Serial.println("*");
      beep();
      digitalWrite(NightLED, HIGH);
      password[i--] = ' ';
    }
    else if(key == '#'){
     Serial.println("#"); 
      password[i--] = ' ';
    }

    else {
      digitalWrite(NightLED, LOW);
      lcd.print(key);
      Serial.print(key);
    }
  }
  if (mode == 1) {
    elapsed_time_s = time(millis() / 1000);
    Serial.println(elapsed_time_s);
    if (elapsed_time_s < 10) {
      digitalWrite(NightLED, HIGH);
      Serial.println("Night LED ON");
    }
    else {
      timer0_millis = 0;
      digitalWrite(NightLED, LOW);
      Serial.println("Night LED OFF");
      mode = 0;
    }
  }
  if (i == 6)
  {
    delay(200);
    for (int j = 0; j < 6; j++)
      pass[j] = EEPROM.read(j);
    if (!(strncmp(password, pass, 6)))
    {
      
      setLocked (false);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("*** Verified ***");
      Serial.println("*** Verified ***");
      ServoMotor.write(0);
      digitalWrite(greenLED, HIGH);
      digitalWrite(blueLED, LOW);
      delay(2000);

      // delay(2000);
      lcd.setCursor(0, 1);
      lcd.print("#.Change Password");
      Serial.println("#.Change Password");
      delay(2000);
      lcd.clear();
      ServoMotor.write(90);
      digitalWrite(greenLED, LOW);
      digitalWrite(blueLED, LOW);
      lcd.print("Enter Password:");
      Serial.println("Enter Password:");
      lcd.setCursor(0, 1);
      i = 0;

    }
    else
    {
      ServoMotor.write(90);
      lcd.clear();
      lcd.print("Access Denied...");
      Serial.println("Access Denied...");
      lcd.setCursor(0, 1);
      lcd.print("#.Change Password");
      Serial.println("#.Change Password");
      alarmOn();
      delay(2000);
      lcd.clear();
      lcd.print("Enter Password:");
      Serial.println("Enter Password:");
      lcd.setCursor(0, 1);
      i = 0;
      digitalWrite(greenLED, LOW);
      digitalWrite(blueLED, LOW);
      
    }
  }
}

void change()
{
  ServoMotor.write(90);
  int j = 0;
  lcd.clear();
  lcd.print("Your Curnt Password");
  Serial.println("Your Curnt Password");
  lcd.setCursor(0, 1);
  while (j < 6)
  {
    char keys = getKey();
    if (keys)
    {
      if(keys == '#'){
     // Serial.println("Do Nothing");
      }
      else{
      pass1[j++] = keys;
      lcd.print(keys);
      Serial.print(keys);
      }

      beep();
    }
    keys = 0;
  }
  delay(500);

  if ((strncmp(pass1, pass, 6)))
  {
    lcd.clear();
    lcd.print("Wrong Password...");
    Serial.println("Wrong Password...");
    lcd.setCursor(0, 1);
    lcd.print("Better Luck Again");
    Serial.println("Better Luck Again");
    alarmOn();
    delay(1000);
  }
  else
  {
    j = 0;
    lcd.clear();
    lcd.print("Enter New Password:");
    Serial.println("Enter New Password:");
    lcd.setCursor(0, 1);
    while (j < 6)
    {
      char keys = getKey();
      if (keys)
      {
       if(keys == '#'){
      //Serial.println("Do Nothing");
      }
       else{
       pass[j] = keys;
       lcd.print(keys);
       Serial.print(keys);
       EEPROM.write(j, keys);
       j++;
       beep();
       }
      }
    }
    digitalWrite(blueLED, HIGH);
    digitalWrite(greenLED, LOW);
    lcd.print(" Done......");
    Serial.println(" Done......");
    delay(2000);
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    //delay(1000);
  }
  lcd.clear();
  lcd.print("Enter Ur Password:");
  Serial.println("Enter Ur Password:");
  lcd.setCursor(0, 1);
  //key=0;
  
}

void setLocked(int locked) {
  if (locked) {
    Serial.println("Locked");
    digitalWrite(greenLED, LOW);
    digitalWrite(blueLED, LOW);
  }
  else {
    Serial.println("Unocked");
    digitalWrite(greenLED, HIGH);
    digitalWrite(blueLED, LOW);
  }
}

unsigned int getKey() {
  keyVal1 = analogRead(keyPadAnalogPin);
  delay(50);
  keyVal2 = analogRead(keyPadAnalogPin);
  if(keyVal1 == keyVal2) {
    return getMatrixValue(keyVal1);
  }
  else
    return NO_KEY;
}
//char getMatrixValue(int val) {
//  switch(val) {
//  case 844 : return keys[0][0];
//  case 904 : return keys[0][1];
//  case 872 : return keys[0][2];
//  case 783 : return keys[1][0];
//  case 831 : return keys[1][1];
//  case 804 : return keys[1][2];
//  case 718 : return keys[2][0];
//  case 808 : return keys[1][3];
//  case 757 : return keys[2][1];
//  case 736 : return keys[2][2];
//  case 667 : return keys[3][0];
//  case 701 : return keys[3][1];
//  case 682 : return keys[3][2];
//  case 719 : return keys[3][1];
//  case 708 : return keys[3][2];
//  case 698 : return keys[3][3];
//  default : return NO_KEY;
//  Serial.println(val);
//  }
//}
char getMatrixValue(int val) {
  if ((val>919) && (val<922)){
    //Serial.println("key 1");
    return keys[0][0];
    }
  else if ((val>900) && (val<910)){
    //Serial.println("key 2");
    return keys[0][1];
    }
  else if ((val>865) && (val<875)){
   // Serial.println("key 3");
    return keys[0][2];
    }
  else if ((val>836) && (val<842)){
   // Serial.println("key 4");
    return keys[1][0];
    }
  else if ((val>819) && (val<824)){
   // Serial.println("key 5");
    return keys[1][1];
    }
  else if ((val>790) && (val<800)){
   // Serial.println("key 6");
    return keys[1][2];
    }
  else if ((val>770) && (val<780)){
   // Serial.println("key 7");
    return keys[2][0];
    }
  else if ((val>758) && (val<763)){
   // Serial.println("key 8");
    return keys[2][1];
    }
  else if ((val>736) && (val<741)){
   // Serial.println("key 9");
    return keys[2][2];
    }
  else if ((val>715) && (val<724)){
   // Serial.println("key *");
    return keys[3][0];
    }
  else if ((val>703) && (val<713)){
   // Serial.println("key 0");
    return keys[3][1];
    }
  else if ((val>685) && (val<695)){
   // Serial.println("key #");
    return keys[3][2];
    }
  return NO_KEY;
}

void beep()
{
  int duracion = 100;
  tone(buzzer, 600, duracion);
  delay(50);
  noTone(buzzer); 
  delay(50);
}

void alarmOn()
{
  lcd.clear();
  lcd.print("ALARM ON");
  Serial.println("ALARM ON");
  tone(buzzer, 900, 5000);
  delay(5000);
  noTone(buzzer); 
  delay(50);
}

long time(long val) {
  int second = numberOfSeconds(val);
  return second;
}

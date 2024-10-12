#include <Stepper.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ButtonControl.h>
LiquidCrystal_I2C lcd(0x27, 24, 4); 

const int stepsPerRevolution = 2048; // Số bước / 1 vòng quay đối với động cơ bước 28BYJ-48

Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11); // Cài đặt động cơ bước và cấu hình chân cắm.
//Stepper myStepper1 = Stepper(stepsPerRevolution, 10, 8, 9, 11);

//button door
const int buttonPin = 7;
const int buttonPin2 = 5;

//Button control
ButtonControl btnconfig(4);
ButtonControl btnfordelayandon_off(3);
ButtonControl btnrst(2);

//Buzzer 
const int buzzerPin = 6;

//Button State
int buttonState = 0;
int buttonState2 = 0;

//Delay of door
int delayTime = 8000; 
int realTime = delayTime/1000;

//Off mode
bool offMode = false; 

void setup()  
{
  lcd.init();                    
  lcd.backlight();
  myStepper.setSpeed(18);
  pinMode(13, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  pinMode(buzzerPin, OUTPUT);
  Serial.begin(115200);
}

void loop() 
{
  if(digitalRead(12) == 0)
  {
    Serial.println("Trang thai ranh, Delay:" + String(realTime));
    goodbye_lcd();
    digitalWrite(13, LOW);
    myStepper.step(-0.25 * stepsPerRevolution);
    lcd.clear();
    while(digitalRead(12) == 0)
    {
      //Read button Pin (1, 2)
      buttonState = digitalRead(buttonPin);
      buttonState2 = digitalRead(buttonPin2);

      //Button for open door
      if(buttonState == LOW || buttonState2 == LOW)
      {
        door_reset();
      }
      //Button for Config
      if(btnconfig.longClick())
      {
        if(btnconfig.getLongClickTime()>1000)
        {
          Serial.println("Dang o che do Config");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("1 lan: Delay");
          lcd.setCursor(0, 1);
          lcd.print("2 lan: On/off");
          
          while(true){
            //Delay Mode
            if(btnfordelayandon_off.click())
            {
              if(btnfordelayandon_off.getButtonClicks() == 1)
              {
                Serial.println("Dang o che do delay");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Che do Delay");
                lcd.setCursor(0, 1);
                lcd.print("Hien tai: " + String(realTime));
                //Set up time for Delay of door
                while(true)
                {
                  if(btnfordelayandon_off.click())
                  {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Delay:" + String(btnfordelayandon_off.getButtonClicks()) + "s");
                    delayTime = btnfordelayandon_off.getButtonClicks()*1000;
                    realTime = delayTime/1000;
                    break;
                  }
                  //Reset Button
                  else if(btnrst.click())
                  {
                    if(btnrst.getButtonClicks() == 1)
                    {
                      Serial.println("Thoat che do config");
                      lcd.clear();
                      break;
                    }
                    else if(btnrst.getButtonClicks() == 2)
                    {
                      Serial.println("Dat ve mac dinh");
                      lcd.clear();
                      lcd.setCursor(0, 0);
                      lcd.print("Default (8s)");
                      delayTime = 8000;
                      realTime = delayTime/1000;
                      break;
                    }
                  }
                }
                break;
              }
              else if(btnfordelayandon_off.getButtonClicks() == 2)
              {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("1 lan: On");
                lcd.setCursor(0, 1);
                lcd.print("2 lan: Off");
                while(true)
                {
                  if(btnfordelayandon_off.click())
                  {
                    if(btnfordelayandon_off.getButtonClicks()==1)
                    {
                      Serial.println("On Auto!");
                      lcd.clear();
                      lcd.setCursor(0, 0);
                      lcd.print("On Mode!");
                      offMode = false;
                      break;
                    }
                    else if(btnfordelayandon_off.getButtonClicks()==2)
                    {
                      Serial.println("Off Auto!");
                      lcd.clear();
                      lcd.setCursor(0, 0);
                      lcd.print("Off Mode!");
                      offMode = true;
                      while(offMode == true)
                      {
                        digitalRead(12) == 0;
                        
                        buttonState = digitalRead(buttonPin);
                        buttonState2 = digitalRead(buttonPin2);

                        if(buttonState == LOW || buttonState2 == LOW)
                        {
                          door_reset();
                        }
                        else if(btnrst.click())
                        {
                          Serial.println("Thoat che do config");
                          lcd.clear();
                          break;
                        }
                      }
                      break;
                    }                   
                  }
                else if(btnrst.click())
                  {
                      Serial.println("Thoat che do config");
                      lcd.clear();
                      break;
                  }
                }
                break;
              }
            }
            else if(btnrst.click())
            {
              Serial.println("Thoat che do config");
              lcd.clear();
              break;
            }
          }
        }
      }
    };
  }

  else if(digitalRead(12) == 1)
  {
    digitalWrite(13, HIGH);
    buzzerandDoorIRQ_ON();
    myStepper.step(0.25 * stepsPerRevolution); 
    while(digitalRead(12) == 1);
    //Sử dụng vòng lặp trống để hạn chế việc lặp lại chương trình
    delay(delayTime);
  }
}

void welcome_lcd()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Xin chao!");
}

void goodbye_lcd()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tam biet!");
}

void buzzerandDoorIRQ_ON()
{
  Serial.println("Cua dang mo");
  welcome_lcd();
  digitalWrite(buzzerPin, 1);
  delay(500);
  digitalWrite(buzzerPin, 0);
}

void door_reset()
{
  digitalWrite(13, HIGH);
  myStepper.step(0.25 * stepsPerRevolution); 
  delay(3000);
  digitalWrite(13, LOW);
  myStepper.step(-0.25 * stepsPerRevolution);
}


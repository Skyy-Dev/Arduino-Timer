#include <Arduino.h>
#include <LiquidCrystal.h>

//PINS

//LEDS
#define RED 8

//buttons
#define leftbuttonApin 9
#define secondbuttonApin 12
#define fivebuttonApin 10
#define tenbuttonApin 11

//buzzer
#define buzzer 13

int len = 12;
//LCD init
LiquidCrystal lcd_1(7, 6, 5, 4, 3, 2);

// put function declarations here:
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(RED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  pinMode(leftbuttonApin, INPUT_PULLUP); //Reset
  pinMode(secondbuttonApin, INPUT_PULLUP); //Start

  pinMode(fivebuttonApin, INPUT_PULLUP);
  pinMode(tenbuttonApin, INPUT_PULLUP);
  
  digitalWrite(RED, LOW);
  digitalWrite(buzzer, LOW);

  lcd_1.begin(16, 2);
  //lcd_1.clear();
  lcd_1.print("Is it time?");

  Serial.begin(9600);
}

int localminutes = 0;

int localseconds = 0;

bool off = false;

bool done;

int laststate = LOW;

unsigned long delayStart = 0;
bool delayRunning = false;

unsigned long delayTime;

int fivemins = 0;
int tenmins = 0;
int reset = 0;
int start = digitalRead(secondbuttonApin);

bool pressed = false;
bool running = false;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int lastbuttonstate = HIGH;
int buttonstate = HIGH;

bool endedval = false;


void timefive()
{
  fivemins = digitalRead(fivebuttonApin);

  if (fivemins == LOW)
    {
      //digitalWrite(RED, HIGH);
      lcd_1.setCursor(6,1);
      lcd_1.print(localminutes); 
      lcd_1.print(":00");

      localminutes = 0;
      localseconds = 10;

      //time = 5000;
      //lcd_1.print(delayTime);
    }
}

void timeten()
{
  tenmins = digitalRead(tenbuttonApin);

  if (tenmins == LOW)
    {
      //digitalWrite(RED, HIGH);
      lcd_1.setCursor(6,1);
      lcd_1.print(localminutes); 
      lcd_1.print(":00");

      localminutes = 10;
      localseconds = 0;

      //time = 5000;
      //lcd_1.print(delayTime);
    }
}

void ended()
{
  if (endedval)
  {
    digitalWrite(buzzer, HIGH);
    digitalWrite(RED, HIGH);
  }
  else
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(RED, LOW);
  }

}

void timer()
{
  if (running)
  {
    lcd_1.setCursor(6, 1);
    lcd_1.print(localminutes);
    lcd_1.print(":");
    lcd_1.print(localseconds < 10 ? "0" : "");
    lcd_1.print(localseconds);

    localseconds--;

    if (localseconds < 0)
    {
      localminutes--;
      localseconds = 59;
    }
    if (localseconds > 59)
    {
      localseconds = 59;
    }
    if (localminutes <= 0 && localseconds <= 0)
    {
      localminutes = 0;
      localseconds = 0;
      digitalWrite(buzzer, HIGH);
      digitalWrite(RED, HIGH);

      lcd_1.clear();
      lcd_1.begin(16, 2);
      lcd_1.print("DONE, GET FOOD");

      running = false;

    }
    if (localminutes < 0 && running == false)
    {

    }
    if (localminutes >= 10 && localseconds == 0) //probably up for deletion
    {
      localminutes--;
    }

    delay(1000);
  }
  
  
}

// the loop function runs over and over again forever
void loop() 
{
  while (Serial.available() == 0)
  { 
    //fivemins = digitalRead(fivebuttonApin);
    start = digitalRead(secondbuttonApin);
    reset = digitalRead(leftbuttonApin);

    if (digitalRead(fivebuttonApin) == LOW)
    {
      timefive();

      //backup
      digitalWrite(buzzer, LOW);
      digitalWrite(RED, LOW);
    }

    if (digitalRead(tenbuttonApin) == LOW)
    {
      timeten();

      digitalWrite(buzzer, LOW);
      digitalWrite(RED, LOW);
    }


    //Timer debouncing

    if (start != laststate)
    {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
      if (start != buttonstate)
      {
        buttonstate = start;

        if (buttonstate == LOW && !pressed)
        {
          pressed = true;
          running = !running;
    
          if (running)
          {
            lcd_1.clear();
          }
          else
          {
            digitalWrite(buzzer, LOW);
            digitalWrite(RED, LOW);
          }
        }
      }
    }

    //Running
    laststate = start;
    timefive();
    timeten();
    ended();
    timer();
    
  }

  

  while (Serial.available() > 0)
  { 
    lcd_1.clear();
  
    String min = Serial.readStringUntil('\n');
    int minute = min.toInt();

    if (digitalRead(leftbuttonApin) == LOW)
    {
      off = true;
    }

    lcd_1.print(minute);

    //if (minute != '\n' && (pos < len -1))
    //{
      //message[len] = minute;
      //pos++;
    //}
    //else
    //{
      //message[len] = '\0';
      //pos = 0;
    //}
    

    if (minute > 1 && off == false)
    {
      digitalWrite(RED, LOW);
      digitalWrite(buzzer, LOW);
    }
    else
    {
      if (off == true)
      {
        digitalWrite(buzzer, LOW);
        digitalWrite(RED, LOW);

        lcd_1.clear();
        lcd_1.print("Another one?");

        off = false;
      }
      else
      {
        digitalWrite(RED, HIGH);
        digitalWrite(buzzer, HIGH);
      }
    }
    //digitalWrite(LED_BUILTIN, HIGH);
  }


  //digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
}
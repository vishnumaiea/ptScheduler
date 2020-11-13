
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blockcing
// periodic tasks for Arduino without using traditional NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.1
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 10:48:05 PM 13-11-2020, Friday

//=======================================================================//
//defines

#define LED1 LED_BUILTIN
#define LED2 12

//=======================================================================//
//globals

ptScheduler sayHello = ptScheduler(1000);
ptScheduler sayName = ptScheduler(-3000);

ptScheduler basicBlink = ptScheduler(1000);
ptScheduler multiBlink = ptScheduler(100);

uint8_t ledOn = false;

//=======================================================================//

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.print("\n-- ptScheduler --\n\n");
}

//=======================================================================//

void loop() {
  //executed every second
  if (sayHello.call()) {
    Serial.println("Hello World");
  }
  
  //skips first time and executed every 3 seconds
  if (sayName.call()) {
    Serial.println("I am ptScheduler");
    multiBlink.activate();
  }
  
  //toggles LED every second
  if (basicBlink.call()) {
    digitalWrite (LED1, !digitalRead(LED1));
  }
  
  //task as a function
  blinker();
}

//=======================================================================//

void blinker() {
  if (multiBlink.call()) {
    if (multiBlink.taskCounter == 1) {
      ledOn = true;
    }
    else {
      ledOn = !ledOn;
    }

    if (ledOn) {
      digitalWrite (LED2, HIGH);
    }

    else {
      digitalWrite(LED2, LOW);
    }

    if (blinkLED.taskCounter == 6) {
      blinkLED.deactivate();
      blinkLED.taskCounter = 0;
      digitalWrite(LED2, LOW);
    }
  }
}

//=======================================================================//


#include <Keypad.h>
#include <Adafruit_MLX90614.h> //for Themperature Sensor Library
#include <Adafruit_ST7735.h> //for TFT Display Libray
#include <String.h>
#include <SPI.h>

#define TFT_CS  44
#define TFT_RST 26
#define TFT_DC  45

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = { 36, 38, 40, 42 };
byte colPins[COLS] = { 28, 30, 32, 34 };


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Adafruit_MLX90614 suhu = Adafruit_MLX90614();
int max_digit=3,timer,minutes,seconds=0;
String tempTimer;
String timerInput;
String timeDisplay;
String Status="COFFEE ROASTER";
bool isSetTimer=0,isTimerStart=0,interruptTimer=0;
char state;
int x=0;
int isValveOpen=0;
bool tempCondition=0;
float themp;

const int VALVE_PIN = 22;
const int ALARM_PIN = 24;

void generalDisplay(){
  //Instruction
  tft.fillRect(0, 33, 150, 40, ST7735_WHITE);  // Draw filled rectangle (x,y,width,height,color)
  tft.setCursor(10,43);
  tft.setTextColor(ST7735_BLACK);
  tft.setTextSize(1);
  tft.println("Tekan 'Atur Waktu'");
  tft.setCursor(17,53);
  tft.setTextColor(ST7735_BLACK);
  tft.setTextSize(1);
  tft.println("terlebih dahulu");
}

void alarm(){
  int loopAlarm=0;
  while (loopAlarm<3)
  {
    digitalWrite(ALARM_PIN,HIGH);
    delay(100);
    digitalWrite(ALARM_PIN,LOW);
    delay(100);
    loopAlarm++;
  }
  delay(300);
  digitalWrite(ALARM_PIN,HIGH);
  delay(400);
  digitalWrite(ALARM_PIN,LOW);
  delay(400);
}

void setup(){
    suhu.begin();
    //init valve pin
    pinMode(VALVE_PIN,OUTPUT);
    pinMode(ALARM_PIN,OUTPUT);
    pinMode(2,INPUT_PULLUP);
    digitalWrite(VALVE_PIN,HIGH);
    
    // Init 1.8" TFT
    tft.initR(INITR_BLACKTAB);  // Initialize a ST7735S chip with black tab

    tft.fillScreen(ST7735_BLACK);  // Fill screen with black theme
    //tft.setRotation(1);
    tft.setTextWrap(true);

    //Title
    tft.setCursor(22,7);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Coffee Roaster");
    tft.fillRect(0, 20, 150, 2, ST7735_WHITE);

    //Instruction
    generalDisplay();

    statusValve(0);

    tft.fillRect(0,85, 150, 2, ST7735_WHITE);

    //Tel-U
    tft.fillRect(0, 140, 150, 100, ST7735_WHITE); // Draw filled rectangle (x,y,width,height,color)
    tft.setCursor(12,145);
    tft.setTextColor(ST7735_RED,ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Telkom University");
}

void loop(){
  
  char key = keypad.getKey();
  delay(50);
  themperatureMonitor();
  if (isTimerStart && !interruptTimer)
  {
    if (minutes==0 && seconds==0)
    {
      detachInterrupt(digitalPinToInterrupt(2));
    }else{
      attachInterrupt(digitalPinToInterrupt(2),makeInterrupt,HIGH);
    }
    while (x<1)
    {
      tft.setCursor(10,40);
      tft.setTextColor(ST7735_BLACK);
      tft.setTextSize(1);
      tft.println("ATUR WAKTU (MENIT)");

      tft.setCursor(50,50);
      tft.setTextSize(2);
      tft.println(timeDisplay);
      tft.fillRect(0, 30, 10, 18, ST7735_BLACK); // Draw filled rectangle (x,y,width,height,color)
      ++x;
    }
    timerCountdown();
  }else if(interruptTimer){
    
    confirmStopTimer();
  }else{
    // State SET/RESET TIMER
  if(state=='A'){
    setTimer();
  }
  // State OPEN VALVE
  if(state=='B'){
    openValve();
  }
  // State CLOSE VALVE
  if(state=='C'){
    closeValve();
  }
  // State START TIMER
  if(state=='D'){
    startTimer();
  }

  if(key){
    if(timerInput.length()<2 && isSetTimer){
      
      if(key=='1' || key=='2' || key=='3' || key=='4' || key=='5' || key=='6' || key=='7' || key=='8' || key=='9' || key=='0'){
        timerInput+=key;
      }

      if(key=='A'){
        isSetTimer=0;
        state='A';
        x=0;
      }
  
      if(key=='D'){
        if (timerInput.toInt()>0)
        {
          isSetTimer=0;
          state='D';
          x=0;       
        }else{
          generalDisplay();
          isSetTimer=0;
          state="";
          x=0;
        }
      }
     
    }else{
      if(timerInput.length()>=2){
        isSetTimer=0;
      }
    
      if(key=='A'){
        isSetTimer=0;
        state='A';
        x=0;
      }
  
      if(key=='B'){
        isSetTimer=0;
        state='B';
        x=0;
      }
  
      if(key=='C'){
        isSetTimer=0;
        state='C';
        x=0;
      }
  
      if(key=='D'){
        if (timerInput.toInt()>0)
        {
          isSetTimer=0;
          state='D';
          x=0;       
        }else{
          generalDisplay();
          isSetTimer=0;
          state="";
          x=0;
        }
      }
    } 
  }
  
  if (isSetTimer)
  {
    if (timerInput.length()<1)
    {
      timeDisplay = "00";
    }
    if (timerInput.length()==1)
    {
      timeDisplay = "0" + timerInput;
    }
    if (timerInput.length()==2)
    {
      timeDisplay = timerInput;
    }
    tft.setCursor(10,40);
    tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
    tft.setTextSize(1);
    tft.println("ATUR WAKTU (MENIT)");

    tft.setCursor(50,50);
    tft.setTextSize(2);
    tft.println(timeDisplay);
    tft.fillRect(0, 30, 10, 18, ST7735_BLACK); // Draw filled rectangle (x,y,width,height,color)
  }
  }
  
}

void makeInterrupt(){
  interruptTimer=1;
  x=0;
}

void confirmStopTimer(){
  
  //tft.fillRect(10, 60, 12, 18, ST7735_BLACK); // Draw filled rectangle (x,y,width,height,color)
  while (x<1)
  {
    tft.setCursor(50,40);
    tft.setTextColor(ST7735_BLACK, ST7735_BLACK);

    tft.setTextSize(1);
    tft.println("WAKTU");
    tft.setCursor(35,50);
    tft.setTextSize(2);
    tft.println(tempTimer);
    tft.fillRect(0,30,150,50,ST7735_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10,40);
    tft.setTextColor(ST7735_WHITE);
    tft.println("BERHENTI ROASTING?");
    confirmButton(1);
    ++x;
  }
  
  char key = keypad.getKey();

  if (key)
  {
    if(key=='#'){
      tft.fillRect(0,30,150,50,ST7735_BLACK);
      tft.setTextSize(1);
      tft.setCursor(12,40);
      tft.setTextColor(ST7735_BLACK);
      tft.println("BERHENTI ROASTING?");
      x=0;
      isValveOpen=0;
      statusValve(isValveOpen);
      // Y Action
      Status="STOP TIMER";
      isTimerStart=1;
      interruptTimer=0;
      minutes=0;
      seconds=0;
      timerInput="";
      detachInterrupt(digitalPinToInterrupt(2));
      //noInterrupts();
    }else if(key=='*'){
      // N Action
      tft.fillRect(0,30,150,50,ST7735_BLACK);
      tft.setTextSize(1);
      tft.setCursor(12,40);
      tft.setTextColor(ST7735_BLACK);
      tft.println("BERHENTI ROASTING?");
      isTimerStart=1;
      interruptTimer=0;
      x=0;
    }
  }
}

void statusValve(int i){
  tft.setTextSize(1);
  switch (i)
  {
  case 0:
    tft.fillRect(0, 125, 150, 15, ST7735_RED);
    tft.setCursor(5,128);
    tft.setTextColor(ST7735_WHITE);
    tft.println("status: Gas Tertutup");
    digitalWrite(VALVE_PIN,HIGH);
    break;
  case 1:
    tft.fillRect(0, 125, 150, 15, ST7735_GREEN);
    tft.setCursor(7,128);
    tft.setTextColor(ST7735_BLACK);
    tft.println("status: Gas Terbuka");
    digitalWrite(VALVE_PIN,LOW);
    break;
  default:
    break;
  }
}

void themperatureMonitor(){
  themp = suhu.readObjectTempC();
  // themp=random(10,20);
  tft.setCursor(25,95);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.setTextSize(1);
  tft.println("SUHU (CELCIUS)");

  tft.setCursor(35,105);
  tft.setTextSize(2);
  tft.println(themp);
  //tft.fillRect(0,0, 12, 18, ST7735_BLACK); // Draw filled rectangle (x,y,width,height,color)
}

void confirmDisplay(){
  tft.fillRect(0,30,150,50,ST7735_BLACK);
  tft.setTextSize(1);

  switch (state)
  {
  case 'A':
    tft.setCursor(32,40);
    tft.setTextColor(ST7735_WHITE);
    tft.println("ATUR WAKTU?");
    confirmButton(1);
    break;
  case 'B':
    tft.setCursor(15,40);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("BUKA SELANG GAS?");
    confirmButton(1);
    break;
  case 'C':
    tft.setCursor(12,40);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("TUTUP SELANG GAS?");
    confirmButton(1);
    break;
  case 'D':
    tft.fillRect(0,25,150,18,ST7735_WHITE);

    if(timerInput.length()==2)  tft.setCursor(20,30);
    if(timerInput.length()==1)  tft.setCursor(23,30);
    tft.setTextColor(ST7735_BLACK);
    tft.setTextSize(1);
    tft.println("WAKTU: "+timerInput+" menit");

    tft.setCursor(21,50);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("MULAI ROASTING?");
    confirmButton(2);
    break;
  default:
    break;
  }
}

void confirmButton(int choose){
  switch (choose)
  {
  case 1:
    tft.setCursor(40,50);
    tft.setTextColor(ST7735_GREEN);
    tft.println("Y -> IYA");

    tft.setCursor(35,60);
    tft.setTextColor(ST7735_RED);
    tft.println("N -> TIDAK");
    break;
  case 2:
    tft.setCursor(40,60);
    tft.setTextColor(ST7735_GREEN);
    tft.println("Y -> IYA");

    tft.setCursor(35,70);
    tft.setTextColor(ST7735_RED);
    tft.println("N -> TIDAK");
    break;
  default:
    break;
  }
}

void clsConfirmButton(int choose){
  switch (choose)
  {
  case 1:
    tft.setCursor(40,50);
    tft.setTextColor(ST7735_BLACK);
    tft.println("Y -> IYA");

    tft.setCursor(35,60);
    tft.setTextColor(ST7735_BLACK);
    tft.println("N -> TIDAK");
    break;
  case 2:
    tft.setCursor(40,60);
    tft.setTextColor(ST7735_BLACK);
    tft.println("Y -> IYA");

    tft.setCursor(35,70);
    tft.setTextColor(ST7735_BLACK);
    tft.println("N -> TIDAK");
    break;
  default:
    break;
  }
}

void clearDisplay(){
  tft.fillRect(0,30,150,50,ST7735_BLACK);
  tft.setTextSize(1);

  switch (state)
  {
  case 'A':
    tft.setCursor(32,40);
    tft.setTextColor(ST7735_BLACK);
    tft.println("ATUR WAKTU?");
    clsConfirmButton(1);
    break;
  case 'B':
    tft.setCursor(15,40);
    tft.setTextColor(ST7735_BLACK);
    tft.setTextSize(1);
    tft.println("BUKA SELANG GAS?");
    clsConfirmButton(1);
    break;
  case 'C':
    tft.setCursor(12,40);
    tft.setTextColor(ST7735_BLACK);
    tft.setTextSize(1);
    tft.println("TUTUP SELANG GAS?");
    clsConfirmButton(1);
    break;
  case 'D':
    tft.fillRect(0,25,150,18,ST7735_BLACK);

    if(timerInput.length()==2)  tft.setCursor(20,30);
    if(timerInput.length()==1)  tft.setCursor(23,30);
    tft.setTextColor(ST7735_BLACK);
    tft.setTextSize(1);
    tft.println("WAKTU: "+timerInput+" menit");

    tft.setCursor(21,50);
    tft.setTextColor(ST7735_BLACK);
    tft.setTextSize(1);
    tft.println("MULAI ROASTING?");
    clsConfirmButton(2);
    break;
  default:
    break;
  }
}

void setTimer(){
  while (x<1)
  {
    confirmDisplay();
    ++x;
  }
  
  char key = keypad.getKey();

  if (key)
  {
    if(key=='#'){
      clearDisplay();
      x=0;
      // Y Action
      Status="SET TIMER";
      isSetTimer=1;
      isTimerStart=0;
      timerInput="";
      state="";
    }else if(key=='*'){
      // N Action
      clearDisplay();
      generalDisplay();
      isSetTimer=0;
      state="";
      x=0;
    }
  }
}

void openValve(){
  while (x<1)
  {
    confirmDisplay();
    ++x;
  }

  char key = keypad.getKey();

  if (key)
  {
    if(key=='#'){
      // Y Action
      clearDisplay();
      generalDisplay();
      Status="VALVE OPENED";
      isValveOpen=1;
      statusValve(isValveOpen);
      state="";
      x=0;
    }else if(key=='*'){
      // N Action
      clearDisplay();
      generalDisplay();
      Status="COFFEE ROASTER";
      state="";
      x=0;
    }
  }
}

void closeValve(){
  while (x<1)
  {
    confirmDisplay();
    ++x;
  }

  char key = keypad.getKey();

  if (key)
  {
    if(key=='#'){
      // Y Action
      clearDisplay();
      generalDisplay();
      Status="VALVE CLOSED";
      isValveOpen=0;
      statusValve(isValveOpen);
      state="";
      x=0;
    }else if(key=='*'){
      // N Action
      clearDisplay();
      generalDisplay();
      Status="COFFEE ROASTER";
      state="";
      x=0;
    }
  }
}

void startTimer(){
  while (x<1)
  {
    confirmDisplay();
    ++x;
  }
  char key = keypad.getKey();
  if (key)
  {
    if(key=='#'){
      minutes=timerInput.toInt();
      isValveOpen=1;
      statusValve(isValveOpen);
      clearDisplay();
      x=0;
      tft.setCursor(10,40);
      tft.setTextColor(ST7735_BLACK, ST7735_BLACK);
      tft.setTextSize(1);
      tft.println("ATUR WAKTU (MENIT)");

      tft.setCursor(50,50);
      tft.setTextSize(2);
      tft.println(timeDisplay);
      tft.fillRect(0, 30, 10, 18, ST7735_BLACK); // Draw filled rectangle (x,y,width,height,color)
      // Y Action
      Status="START TIMER";
      seconds=0;
      isTimerStart=1;
      state="";
    }else if(key=='*'){
      // N Action
      clearDisplay();
      generalDisplay();
      state="";
      x=0;
    }
  }
}

void timerCountdown(){
    tempTimer;
    if(seconds<10 && minutes<10){
      tempTimer = "0" + String(minutes) + ":0" + String(seconds);
    }else if(minutes<10){
      tempTimer = "0" + String(minutes) + ":" + String(seconds);
    }else if(seconds<10){
      tempTimer = String(minutes) + ":0" + String(seconds);
    }else if(minutes==0 && seconds<2){
      tempTimer = "00:00";
    }
    else{
      tempTimer = String(minutes) + ":" + String(seconds);
    }
    seconds--;
    //seconds
    delay(1000);
    
    if(seconds<0 && minutes>=0){
        minutes--;
        seconds=59;
    } 
    if (minutes<0)
    {
        tft.setCursor(50,40);
        tft.setTextColor(ST7735_RED, ST7735_BLACK);
        tft.setTextSize(1);
        tft.println("WAKTU");
        tft.setCursor(25,50);
        tft.setTextSize(2);
        tft.println("SELESAI");
        tft.fillRect(0, 0, 12, 18, ST7735_BLACK); // Draw filled rectangle (x,y,width,height,color)
        alarm();
        isTimerStart=0;
        isValveOpen=0;
        statusValve(isValveOpen);
    }else{

        tft.setCursor(50,40);
        
        if (minutes<1 && seconds<10)
        {
          tft.setTextColor(ST7735_RED, ST7735_BLACK);
        }else tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
        
        tft.setTextSize(1);
        tft.println("WAKTU");
        tft.setCursor(35,50);
        tft.setTextSize(2);
        tft.println(tempTimer);
        tft.fillRect(10, 60, 12, 18, ST7735_BLACK); // Draw filled rectangle (x,y,width,height,color)
    }
}

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <NewPing.h> 
#define MAX_SPD 250
#define MIN_SPD 250
#define MAX_T_SPD 150
#define MIN_T_SPD 150
#define KP 1
#define KI 0.001
#define KD 4
#define NUM_SENSORS 5
#define enMotor 6
#define lpwm 3
#define rpwm 6
#define lmf 5
#define lmb 9
#define rmf 10
#define rmb 11
#define DEBUGGING digitalRead(12)

struct node{
  char dir;
  int time_ms;
  node *next;
};

int max_spd=MIN_SPD, tSpd=MIN_T_SPD;
node* seq=NULL;
int length_of_node=0;
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);
NewPing sonarF(2,2,400), sonarL(7,7,400), sonarR(8,8,400);
int smin[5]={1023,1023,1023,1023,1023},smax[5]={0,0,0,0,0}, s[5]={A6,A3,A2,A1,A0};
String patt,lastPatt;
long pos,lastPos=0;
char line='B', hand='R', dir='S';

byte bar[8][8]={
  {0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111},
  {0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111},
  {0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111,0b11111},
  {0b00000,0b00000,0b00000,0b00000,0b11111,0b11111,0b11111,0b11111},
  {0b00000,0b00000,0b00000,0b11111,0b11111,0b11111,0b11111,0b11111},
  {0b00000,0b00000,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111},
  {0b00000,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111},
  {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111}
};

void calibrate();
String getPattern();long getPos();int dS(int);
void setMotors(int,int);void pid(bool=false);
void turn(char,int=0);
void halt();void softBreak();void inch();void lineGap(int);
void lcdBarCreate();void lcdBarPrint();
String nodeToStr(node*);int* nodeToArr(node*);node* makeNode(node*,char,long);bool hasU(node*);void findShortest(node*);
String str_rev(String);int* arr_rev(int*);

void setup() {
  pinMode(lmf,OUTPUT);pinMode(lmb,OUTPUT);pinMode(rmf,OUTPUT);pinMode(rmb,OUTPUT);pinMode(lpwm,OUTPUT);pinMode(rpwm,OUTPUT);
  pinMode(12,INPUT);pinMode(LED_BUILTIN,OUTPUT);pinMode(4,OUTPUT);
  Serial.begin(9600);
  lcd.begin(16,2);
  lcdBarCreate();lcd.noBacklight();
  digitalWrite(enMotor,HIGH);
  digitalWrite(4,LOW);
  halt();
  calibrate();
  lcd.backlight();
  lcd.setCursor(0,0);lcd.print("Press to confirm");
  while(1){
    lcd.setCursor(0,1);lcd.print("    hand = R    ");delay(1000);
    if(DEBUGGING){hand='R';break;}
    lcd.setCursor(0,1);lcd.print("    hand = L    ");delay(1000);
    if(DEBUGGING){hand='L';break;}
  }
  lcd.setCursor(0,0);lcd.print("   ACTIVATED:   ");
  delay(1000);
}

void loop() {
  node *tmp=NULL;
  String str="";
  int i=0, *timeArr=NULL;
  bool lcdBl=true, back=true, shortest=false, accelerate=true;
  long prevMillis=millis(),currMillis=0, stepMillis=0;
  while(1){
    if(DEBUGGING){
      delay(100);
      dir='S';
      lcd.clear();lcd.setCursor(0,0);
      lcd.print(hand);lcd.print(' ');
      lcd.print(nodeToStr(seq));
      lcd.setCursor(15,0);lcd.print(i);
      if(!lcdBl){lcd.backlight();lcdBl=true;}
    }else{
      if(lcdBl){lcd.noBacklight();lcdBl=false;}
    }
    pos=getPos();
    lastPos=pos;
    patt=getPattern();
    if(DEBUGGING)continue;

    if(patt=="10100"||patt=="10110"){
      setMotors(100,100);delay(100);
      halt();delay(50);
      turn('L',90);turn('L');delay(50);
    }else if(patt=="00101"||patt=="01101"){
      setMotors(100,100);delay(100);
      halt();delay(50);
      turn('R',90);turn('R');delay(50);
    }

    if(shortest){
      currMillis=millis()-prevMillis;
      if(str[i]!='S' && timeArr[i]/2 < currMillis)accelerate=false;
      else accelerate=true;
      if(patt=="00000"||patt=="11111"||patt=="01110"||patt=="01111"||patt=="00111"||patt=="11100"||patt=="11110"){
        if(str[i]==0){
          halt();
          lcd.clear();lcd.backlight();
          lcd.setCursor(0,0);lcd.print(str);
          i=0;
          while(!DEBUGGING);
          lcd.setCursor(0,1);lcd.print('^');
          lcd.setCursor(11,0);lcd.print(timeArr[i]);
          while(DEBUGGING);delay(1000);
          max_spd=MAX_SPD;
          prevMillis=millis();
          continue;
        }else if(str[i]=='S'){
          setMotors(max_spd,max_spd);delay(50);
        }else{
          setMotors(250,250);delay(100); // 50~100
          halt();delay(90); //50~100
          turn(str[i],90);turn(str[i]);delay(50);
        }
        i++;
        lcd.setCursor(11,0);lcd.print(timeArr[i]);
        lcd.setCursor(i,1);lcd.print('^');
//        delay(2000);
        max_spd=MAX_SPD;
        prevMillis=millis();
      }else{
        if(accelerate){
          if(millis()-stepMillis > 10){
            max_spd+=5;
            max_spd=min(max_spd,255);
            stepMillis=millis();
          }
        }else max_spd=MAX_SPD;
        pid();
      }
      continue;
    }
    
    if(hand=='R'){
      if(patt=="11110"){delay(5);patt=getPattern();}
      if(patt=="11100"||patt=="11110"){
        setMotors(200,200);delay(150);
        currMillis=millis()-prevMillis-100;
        if(getPattern()=="00000"){
          halt();delay(100);turn('L');delay(tSpd/2);
          seq=makeNode(seq,'L',currMillis);
          prevMillis=millis();
        }else{
          seq=makeNode(seq,'S',currMillis);
          prevMillis=millis();
          continue;
        }
        max_spd=MAX_SPD;
      }else if(patt=="00111"||patt=="01111"||patt=="11111"){
        setMotors(200,200);delay(150);
        currMillis=millis()-prevMillis-100;
        halt();delay(100);
        if(getPattern()=="11111"){
          shortest=true;
          max_spd=MAX_SPD;
          tSpd=MAX_T_SPD; // better with grip
          lcd.clear();lcd.backlight();
          findShortest(seq);
          str=nodeToStr(seq);
          timeArr=nodeToArr(seq);
          timeArr[length_of_node]=currMillis;
          if(back){
            str=str_rev(str);
            timeArr=arr_rev(timeArr);
            lcd.setCursor(0,0);lcd.print(str);
            lcd.setCursor(0,1);lcd.print('^');
            setMotors(150,150);delay(100);halt();
            delay(1000);digitalWrite(4,HIGH);delay(1000);
            setMotors(-100,-100);delay(200);
            halt();delay(100);
            turn(hand,180);turn(hand);delay(100);
            delay(1000);
            prevMillis=millis();
            continue;
          }else{
            lcd.setCursor(0,0);lcd.print(str);
            while(!DEBUGGING);
            lcd.setCursor(0,1);lcd.print('^');
            while(DEBUGGING);delay(1000);
            prevMillis=millis();
            continue;
          }
        }
        turn('R',90);turn('R');delay(tSpd/2);
        seq=makeNode(seq,'R',currMillis);
        max_spd=MAX_SPD;
        prevMillis=millis();
      }
    }else{ // hand=='L'
      if(patt=="01111"||patt=="00111"){delay(10);patt=getPattern();}
      if(patt=="00111"||patt=="01111"){
        setMotors(200,200);delay(150);
        currMillis=millis()-prevMillis-100;
        if(getPattern()=="00000"){
          halt();delay(100);turn('R');delay(tSpd);
          seq=makeNode(seq,'R',currMillis);
          prevMillis=millis();
        }else{
          seq=makeNode(seq,'S',currMillis);
          prevMillis=millis();
          continue;
        }
        max_spd=MAX_SPD;
      }else if(patt=="11100"||patt=="11110"||patt=="11111"){
        setMotors(200,200);delay(150);
        currMillis=millis()-prevMillis-100;
        halt();delay(100);
        if(getPattern()=="11111"){
          shortest=true;
          max_spd=MAX_SPD;
          lcd.clear();lcd.backlight();
          findShortest(seq);
          str=nodeToStr(seq);
          timeArr=nodeToArr(seq);
          timeArr[length_of_node]=currMillis;
          if(back){
            str=str_rev(str);
            timeArr=arr_rev(timeArr);
            lcd.setCursor(0,0);lcd.print(str);
            lcd.setCursor(0,1);lcd.print('^');
            setMotors(150,150);delay(100);halt();
            delay(1000);digitalWrite(4,HIGH);delay(1000);
            setMotors(-100,-100);delay(200);
            halt();delay(100);
            turn(hand,180);turn(hand);delay(100);
            delay(500);
            prevMillis=millis();
            continue;
          }else{
            lcd.setCursor(0,0);lcd.print(str);
            while(!DEBUGGING);
            lcd.setCursor(0,1);lcd.print('^');
            while(DEBUGGING);delay(1000);
            prevMillis=millis();
            continue;
          }
        }
        turn('L',90);turn('L');delay(tSpd);
        seq=makeNode(seq,'L',currMillis);
        max_spd=MAX_SPD;
        prevMillis=millis();
      }
    }
    
    if(patt=="00000" && 50<pos && pos<350){
      setMotors(200,200);delay(200);
      currMillis=millis()-prevMillis-200;
      halt();delay(100);
      turn(hand,180);turn(hand);delay(tSpd/2);
      seq=makeNode(seq,'U',currMillis);
      max_spd=MAX_SPD;
      prevMillis=millis();
    }else{
        if(millis()-stepMillis > 10){
          max_spd+=5;
          max_spd=min(max_spd,255);
          stepMillis=millis();
        }
        pid();
    }
    
    lastPatt=patt;
  } // endwhile;
}

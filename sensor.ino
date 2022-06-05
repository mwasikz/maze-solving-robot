/*------- VR = 350 ohm & threshold = 60 for 2.5cm line  --------*/
/*------- VR ~ 500 ohm & threshold = 70 for 3cm line  --------*/
int dS(int i)
{
  if(line=='B') return constrain(map(analogRead(s[i]),smin[i],smax[i],0,100), 0,100) < 60?0:1; // no change here
  else return constrain(map(analogRead(s[i]),smin[i],smax[i],0,100), 0,100) > 40?0:1;
}
String getPattern()
{
  String pattern="";
  int dr[5];
  for(int i=0; i<NUM_SENSORS; i++){
    if(line=='B') dr[i]=constrain(map(analogRead(s[i]),smin[i],smax[i],0,100), 0,100) < 60?0:1;
    else dr[i]=constrain(map(analogRead(s[i]),smin[i],smax[i],0,100), 0,100) > 40?0:1;
  }
  for(int i=0; i<NUM_SENSORS; i++){pattern=pattern+dr[i];}
  if(DEBUGGING){
    lcd.setCursor(0,1);lcd.print(pattern);
    Serial.println(pattern);
  }
  return pattern;
}
long getPos()
{
  long pos, ar[5],sum=0;
  for(int i=0; i<NUM_SENSORS; i++){
    if(line=='B') ar[i]=constrain(map(analogRead(s[i]),smin[i],smax[i],-30,100), 0,100);
    else ar[i]=100-constrain(map(analogRead(s[i]),smin[i],smax[i],0,130), 0,100);
  }
  if(DEBUGGING)lcdBarPrint(ar,6,1);
  for(int i=0; i<NUM_SENSORS; i++){sum+=ar[i];}
  pos=(ar[0]* 0 +ar[1]* 100 +ar[2]* 200 +ar[3]* 300 +ar[4]* 400 )/sum;
  if(sum==0)pos=lastPos;
  if(DEBUGGING){lcd.setCursor(12,1);lcd.print(pos);}
  return pos;
}

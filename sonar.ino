bool hasObject()
{
  int front = 0;
  front=sonarF.ping_cm();
  if(front>2 && front<=12)return true;
  else return false;
}
bool hasWall()
{
  int left = 0, right = 0;
  right=sonarR.ping_cm();
  left=sonarL.ping_cm();
  if(right>2 && right<20)return true;
  if(left>2 && left<20)return true;
  return false;
}

void avoidObject(char hand)
{
  int distance, err, lasterr = 0, P, D, spd;
  while (getPattern()=="00000") {
    if(hand=='R')distance = round(sonarL.ping() * 0.343 / 2);
    else distance = round(sonarR.ping() * 0.343 / 2);
    if (distance < 50 && distance != 0)distance = 50;
    if (distance > 150 || distance == 0)distance = 150;
    distance = constrain(map(distance, 50, 150, 0, 100), 0, 100);
    err = distance - 50;
    P = err;
    D = err - lasterr;
    lasterr=err;
    spd = 5 * P + 4 * D;
    if(hand=='R')setMotors(min(max(100 - spd, 25), 100), min(max(100 + spd, 25), 100));
    else setMotors(min(max(100 + spd, 25), 100), min(max(100 - spd, 25), 100));
    delay(10);
  }
  delay(200);
  setMotors(0,0);
}

void followWall(char hand)
{
  int front, left, right, distance, err, lasterr = 0, P, D, spd;
  while (getPattern()=="00000") {
    front=sonarF.ping_cm();
    if(front>2 && front<=10){
      halt();delay(500);
      if(hand=='R'){
        right=sonarR.ping_cm();
        if(right>2 && right<=30){turn('L',90);}else{turn('R',90);}
      }else{
        left=sonarL.ping_cm();
        if(left>2 && left<=30){turn('R',90);}else{turn('L',90);}
      }
      setMotors(100,100);delay(500);
    }
    if(hand=='R')distance = round(sonarR.ping() * 0.343 / 2);
    else distance = round(sonarL.ping() * 0.343 / 2);
    if (distance < 50 && distance != 0)distance = 50;
    if (distance > 150 || distance == 0)distance = 150;
    distance = constrain(map(distance, 50, 150, 0, 100), 0, 100);
    err = distance - 50;
    P = err;
    D = err - lasterr;
    lasterr=err;
    spd = 5 * P + 4 * D;
    if(hand=='R')setMotors(min(max(150 + spd, 25), 150), min(max(150 - spd, 25), 150));
    else setMotors(min(max(150 - spd, 25), 150), min(max(150 + spd, 25), 150));
    delay(10);
  }
  delay(200);
  setMotors(0,0);
}

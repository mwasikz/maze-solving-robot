void calibrate()
{
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  lcd.setCursor(0, 0); lcd.print("Calibrating. . ."); lcd.backlight();
  int a[5], calSpd = 100, calPeriod = 160;
  for (int count = 0; count < calPeriod; count++) {
//    if (count < calPeriod / 4 || count >= calPeriod * 3 / 4) {
//      setMotors(calSpd, -calSpd);
//    } else {
//      setMotors(-calSpd, calSpd);
//    }
    setMotors(calSpd,-calSpd);
    for (int i = 0; i < NUM_SENSORS; i++) {
      a[i] = analogRead(s[i]);
      if (a[i] < smin[i])smin[i] = a[i];
      if (a[i] > smax[i])smax[i] = a[i];
    }
    delay(10);
  }
  setMotors(0, 0);
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print("min["); Serial.print(i); Serial.print("]="); Serial.print(smin[i]);
    Serial.print(", ");
    Serial.print("max["); Serial.print(i); Serial.print("]="); Serial.println(smax[i]);
  }
  digitalWrite(13, LOW);
  lcd.clear(); lcd.print("  PLEASE  WAIT  ");
  delay(2000); lcd.noBacklight();lcd.clear();
}

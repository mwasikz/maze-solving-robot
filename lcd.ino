void lcdBarPrint(long* a, int col, int row)
{
  long v[5];
  for (int i = 0; i < NUM_SENSORS; i++) {
    v[i] = constrain(map(a[i], 0, 100, 0, 9), 1, 8);
    lcd.setCursor(col + i, row);
    lcd.write(v[i]);
  }
}
void lcdBarCreate()
{
  lcd.createChar(1, bar[0]);
  lcd.createChar(2, bar[1]);
  lcd.createChar(3, bar[2]);
  lcd.createChar(4, bar[3]);
  lcd.createChar(5, bar[4]);
  lcd.createChar(6, bar[5]);
  lcd.createChar(7, bar[6]);
  lcd.createChar(8, bar[7]);
}

# import final_python
# import time
# from final_python import initialBatteryLevel_percent, dummyBatteryLevel_percent, processStartTime, lcd, totalSystemPower, totalRenewables

def displayBatteryLevel_charging():
  currentBatteryCharge_kiloJoules = 0
  initialBatteryCharge_kiloJoules = initialBatteryLevel_percent*360/100

  currentBatteryCharge_kiloJoules = initialBatteryCharge_kiloJoules + (time.time()-processStartTime)*(totalRenewables-totalSystemPower)
  currentBatteryLevel_percent =  currentBatteryCharge_kiloJoules/360*100

  if currentBatteryLevel_percent != dummyBatteryLevel_percent and int(currentBatteryLevel_percent - dummyBatteryLevel_percent) == 1:
    lcd.clear()
    lcd.home()
    lcd.write("Charging")
    lcd.setCursor(0, 1)
    lcd.write(currentBatteryLevel_percent)
    lcd.setCursor(3, 1)
    lcd.write("%")
    dummyBatteryLevel_percent = currentBatteryLevel_percent

  elif currentBatteryLevel_percent >= 100:
    lcd.clear()
    lcd.home()
    lcd.write("Fully charged")
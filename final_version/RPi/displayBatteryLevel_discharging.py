# import final_python
# import time
# from final_python import initialBatteryLevel_percent, dummyBatteryLevel_percent, processStartTime, lcd, totalSystemPower, totalRenewables

def displayBatteryLevel_discharging():

    initialBatteryCharge_kiloJoules = initialBatteryLevel_percent*360/100

    currentBatteryCharge_kiloJoules = initialBatteryCharge_kiloJoules - (time.time()-processStartTime)*(totalSystemPower-totalRenewables)
    currentBatteryLevel_percent = currentBatteryCharge_kiloJoules*100/360

    if currentBatteryLevel_percent != dummyBatteryLevel_percent and int(dummyBatteryLevel_percent - currentBatteryLevel_percent) == 1:
        lcd.clear()
        lcd.home()
        lcd.write("Discharging")
        lcd.setCursor(0, 1)
        lcd.write(currentBatteryLevel_percent)
        lcd.setCursor(3, 1)
        lcd.write("%")
        dummyBatteryLevel_percent = currentBatteryLevel_percent

    elif currentBatteryLevel_percent <= 0:
        lcd.clear()
        lcd.home()
        lcd.write("Battery empty.")

    return currentBatteryLevel_percent

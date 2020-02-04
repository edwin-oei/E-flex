import time
import RPi.GPIO as GPIO
import LiquidCrystalPi

GPIO.setmode(GPIO.BOARD)  # BCM = GPIO pin numbering based on broadcom standards


# LCD
lcd = LiquidCrystalPi.LCD(5, 17, 27, 22, 10, 9)
lcd.begin(16, 2)
lcd.write("Hello world")
time.sleep(1.5)
lcd.clear()
GPIO.cleanup()
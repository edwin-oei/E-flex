import time
import RPi.GPIO as GPIO
import LiquidCrystalPi

GPIO.setmode(GPIO.BOARD)  # BCM = GPIO pin numbering based on broadcom standards


# LCD
LCD = LiquidCrystalPi.LCD(29, 31, 33, 35, 37, 38)
LCD.begin(16, 2)
LCD.write("Hello world")
time.sleep(1.5)
LCD.clear()
GPIO.cleanup()
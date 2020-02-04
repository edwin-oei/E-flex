import time
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards


# Pump
pumpPin = 17  # GPIO pin 17 and not RPi pin 17
GPIO.setup(pumpPin, GPIO.OUT)  # Set pin as output pin
GPIO.output(pumpPin, GPIO.HIGH)  # Turn on pump
time.sleep(0.5)
GPIO.output(pumpPin, GPIO.LOW)  # Turn off pump
time.sleep(3)
GPIO.cleanup()
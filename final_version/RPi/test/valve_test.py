import time
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards


# Valve
valvePin = 21   # GPIO pin 20 and not RPi pin 20
GPIO.setup(valvePin, GPIO.OUT)  # Set pin as output pin
GPIO.output(valvePin, GPIO.HIGH)  # Open the valve
time.sleep(1.5)   # Pause for 1 sec
GPIO.output(valvePin, GPIO.LOW)  # Close the valve
GPIO.cleanup()
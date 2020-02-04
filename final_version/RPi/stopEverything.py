# import final_python
# from final_python import lcd, valvePin, pumpPin, motor_1a, motor_1b
# import RPi.GPIO as GPIO
#
# GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards

def stopEverything():
    lcd.clear()
    GPIO.output(valvePin, GPIO.LOW)  # Close the valve
    GPIO.output(pumpPin, GPIO.LOW)  # Turn off the pump
    GPIO.output(motor_1a, GPIO.LOW)  # Stop the conveyor belt
    GPIO.output(motor_1b, GPIO.LOW)
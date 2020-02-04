import time
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards

# Conveyor belt motor
motor_1a = 2 # set GPIO-02 as Input 1 of the controller IC
motor_1b = 3 # set GPIO-03 as Input 2 of the controller IC
conveyorPWM = 4 # set GPIO-04 as Enable pin 1 of the controller IC
GPIO.setup(motor_1a, GPIO.OUT)
GPIO.setup(motor_1b, GPIO.OUT)
GPIO.setup(conveyorPWM, GPIO.OUT)

pwm=GPIO.PWM(4, 100) # configuring Enable pin means GPIO-04 for PWM

pwm.start(50) # starting it with 50% dutycycle or 50% speed
GPIO.output(motor_1a, GPIO.HIGH)
GPIO.output(motor_1b, GPIO.LOW)
GPIO.output(conveyorPWM, GPIO.HIGH)
time.sleep(2)

pwm.ChangeDutyCycle(80) # increasing dutycycle to 80
GPIO.cleanup()
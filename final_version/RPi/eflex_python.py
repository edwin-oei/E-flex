import wiringpi
from time


gpio = wiringpi.GPIO(wiringpi.GPIO.WPI_MODE_GPIO)


# Valve
valvePin = 18   # GPIO pin 18 and not RPi pin 18

gpio.pinMode(valvePin, gpio.OUTPUT)  # Set pin as output pin
wiringpi.pinMode(valvePin, 1)

gpio.digitalWrite(valvePin, gpio.HIGH)  # Open the valve
time.sleep(1)   # Pause for 1 sec
gpio.digitalWrite(valvePin, gpio.LOW)  # Close the valve


# Pump
pumpPin = 17  # GPIO pin 18 and not RPi pin 18

gpio.pinMode(pumpPin, gpio.OUTPUT)  # Set pin as output pin
wiringpi.pinMode(pumpPin, 1)
gpio.digitalWrite(valvePin, gpio.HIGH)  # Turn on the pump
gpio.digitalWrite(valvePin, gpio.LOW)  # Turn off the pump


# Water level sensor
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)

sensorPinOutput = 23
sensorPinInput = 24

GPIO.setup(sensorPinOutput, GPIO.OUT)
GPIO.output(sensorPinOutput, 0)
GPIO.setup(sensorPinInput, GPIO.IN)
time.sleep(0.5)  # Let sensors settle

# The HC-SR04 sensor requires a short 10uS pulse to trigger the module, which will cause the sensor to start the ranging program
# (8 ultrasound bursts at 40 kHz) in order to obtain an echo response.
# So, to create our trigger pulse, we set out trigger pin high for 10uS then set it low again.
GPIO.output(sensorPinOutput, 1)
time.sleep(0.00001)
GPIO.output(sensorPinOutput, 0)

while GPIO.input(sensorPinInput) == 0:
    pass
starTime = time.time()

while GPIO.input(sensorPinInput) == 1:
    pass
endTime = time.time()

pulseDuration = endTime - starTime
waterLevel = pulseDuration*17150


# LCD
LCD = LiquidCrystalPi.LCD(29, 31, 33, 35, 37, 38)
LCD.begin(16, 2)
LCD.write("Hellow world")
time.sleep(1.5)
LCD.clear()


# Conveyor belt motor
motor_1a = 2 # set GPIO-02 as Input 1 of the controller IC
motor_1b = 3 # set GPIO-03 as Input 2 of the controller IC
conveyorPWM = 4 # set GPIO-04 as Enable pin 1 of the controller IC
GPIO.setup(motor_1a, GPIO.OUT)
GPIO.setup(motor_1b, GPIO.OUT)
GPIO.setup(conveyorPWM, GPIO.OUT)

pwm=GPIO.PWM(4, 100) # configuring Enable pin means GPIO-04 for PWM

pwm.start(50) # starting it with 50% dutycycle or 50% speed
GPIO.output(Motor1A,GPIO.HIGH)
GPIO.output(Motor1B,GPIO.LOW)
GPIO.output(Motor1E,GPIO.HIGH)
time.sleep(2)

pwm.ChangeDutyCycle(80) # increasing dutycycle to 80
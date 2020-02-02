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


# LED
from neopixel import *
import argparse

LED_COUNT      = 16      # Number of LED pixels.
LED_PIN        = 18      # GPIO pin connected to the pixels (18 uses PWM!).
#LED_PIN        = 10      # GPIO pin connected to the pixels (10 uses SPI /dev/spidev0.0).
LED_FREQ_HZ    = 800000  # LED signal frequency in hertz (usually 800khz)
LED_DMA        = 10      # DMA channel to use for generating signal (try 10)
LED_BRIGHTNESS = 255     # Set to 0 for darkest and 255 for brightest
LED_INVERT     = False   # True to invert the signal (when using NPN transistor level shift)
LED_CHANNEL    = 0       # set to '1' for GPIOs 13, 19, 41, 45 or 53

def colorWipe(strip, color, wait_ms=50):
    """Wipe color across display a pixel at a time."""
    for i in range(strip.numPixels()):
        strip.setPixelColor(i, color)
        strip.show()
        time.sleep(wait_ms/1000.0)


# Process arguments
parser = argparse.ArgumentParser()
parser.add_argument('-c', '--clear', action='store_true', help='clear the display on exit')
args = parser.parse_args()

# Create NeoPixel object with appropriate configuration.
strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, LED_FREQ_HZ, LED_DMA, LED_INVERT, LED_BRIGHTNESS, LED_CHANNEL)
# Intialize the library (must be called once before other functions).
strip.begin()

print ('Color wipe animations.')
colorWipe(strip, Color(255, 0, 0))  # Red wipe
colorWipe(strip, Color(0, 255, 0))  # Blue wipe
colorWipe(strip, Color(0, 0, 255))  # Green wipe
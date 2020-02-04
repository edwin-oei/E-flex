import time
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards

# Pump settings
pumpPin = 20  # GPIO pin 20 and not RPi pin 20
GPIO.setup(pumpPin, GPIO.OUT)  # Set pin as output pin
GPIO.output(pumpPin, GPIO.LOW)  # Ensure pump is turned off

# Valve settings
valvePin = 21   # GPIO pin 21 and not RPi pin 21
GPIO.setup(valvePin, GPIO.OUT)  # Set pin as output pin
GPIO.output(valvePin, GPIO.LOW)  # Ensure valve is closed

# LCD settings
lcd = LiquidCrystalPi.LCD(29, 31, 33, 35, 37, 38)
lcd.begin(16, 2)

# Light sensor settings
CLK = 6
MISO = 13
MOSI = 19
CS = 26
mcp = Adafruit_MCP3008.MCP3008(clk=CLK, cs=CS, miso=MISO, mosi=MOSI)

# Water level sensor settings
lowerWaterLevelThreshold = 3
upperWaterLevelThreshold = 6
trigPin = 23
echoPin = 24
GPIO.setup(trigPin, GPIO.OUT)
GPIO.output(trigPin, 0)
GPIO.setup(echoPin, GPIO.IN)
time.sleep(0.5)  # Let sensors settle

# Conveyor belt motor
motor_1a = 2  # set GPIO-02 as Input 1 of the controller IC
motor_1b = 3  # set GPIO-03 as Input 2 of the controller IC
conveyorPWM = 4  # set GPIO-04 as Enable pin 1 of the controller IC
GPIO.setup(motor_1a, GPIO.OUT)  # Set pin as output pin
GPIO.setup(motor_1b, GPIO.OUT)
GPIO.setup(conveyorPWM, GPIO.OUT)
pwm=GPIO.PWM(4, 100)  # Configuring enable pin means GPIO-04 for PWM
pwm.start(0)  # Ensure motor is turned off
GPIO.output(motor_1a, GPIO.LOW)
GPIO.output(motor_1b, GPIO.LOW)
GPIO.output(conveyorPWM, GPIO.LOW)

# The values below can be adjusted as how the team sees fit. Power in kW
totalSystemPower = 0.1
maxSolarPower = 0.12
maxWindPower = 0.06
initialBatteryLevel_percent = 50
dummyBatteryLevel_percent = initialBatteryLevel_percent
currentBatteryLevel_percent = initialBatteryLevel_percent
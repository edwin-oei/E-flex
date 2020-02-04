import time
import RPi.GPIO as GPIO
import LiquidCrystalPi
import conveyorBelt
from computeWaterLevel import computeWaterLevel
from stopEverything import stopEverything
from displayBatteryLevel_charging import displayBatteryLevel_charging
from displayBatteryLevel_discharging import displayBatteryLevel_discharging

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


# Pre-cursor to infinite loop
print("Initial battery level: {}%." .format(initialBatteryLevel_percent))

lcd.write("E-Flex demo")
time.sleep(1.5)
lcd.clear()    # Clears the display and lcd memory
lcd.home()  # Brings back cursor to the start of the lcd
lcd.write("Let's start!")
time.sleep(1.5)
lcd.clear()

lcd.write("Battery level")
lcd.setCursor(0, 1)
lcd.write(initialBatteryLevel_percent)
lcd.setCursor(3, 1)
lcd.write("%")

tryLoop = 0
while tryLoop == 0:
    try:
        userDemand = input("Enter water demand:\n(0) No demand\t(1) Low\t(2) High")
        assert (userDemand == '0' or userDemand == '1' or userDemand == '2')
        tryLoop = 1  # Exit try loop
    except AssertionError:
        print("\nError. Please enter either 0, 1 or 2.")

# Print user input as feedback
if userDemand == 0:
    print("\nNo demand")
elif userDemand == 1:
    print("\nLow demand")
elif userDemand == 2:
    print("\nHigh demand")

# Get renewables power from user
tryLoop = 0
while tryLoop == 0:
    try:
        windPower = float(input("Enter wind power (max = 0.06): "))
        assert (0 <= windPower <= maxWindPower)
        lcd.clear()
        lcd.home()
        lcd.write("Wind power")
        lcd.setCursor(0, 1)
        lcd.write(windPower)
        lcd.println(" kW")
        tryLoop = 1  # Exit try loop
    except (AssertionError, ValueError) as error:
        print("Error. Please enter a number between 0 and 0.06.")

tryLoop = 0
while tryLoop == 0:
    try:
        solarPower = float(input("Enter solar power (max = 0.12): "))
        assert (0 <= solarPower <= maxSolarPower)
        lcd.clear()
        lcd.home()
        lcd.write("Wind power")
        lcd.setCursor(0, 1)
        lcd.write(windPower)
        lcd.println(" kW")
        tryLoop = 1  # Exit try loop
    except (AssertionError, ValueError) as error:
        print("Error. Please enter a number between 0 and 0.12.")

time.sleep(1.5)
lcd.clear()
lcd.home()
lcd.write("Total renewables:")
lcd.setCursor(0, 1)
totalRenewables = windPower + solarPower
lcd.write(totalRenewables)
lcd.println(" kW")
time.sleep(2.5)
lcd.clear()
lcd.home()

processStartTime = time.time()

if totalRenewables >= totalSystemPower:
    if totalRenewables == totalSystemPower:
        lcd.write("Battery stable")
    elif totalSystemPower > totalSystemPower:
        lcd.write("Charging")
        lcd.setCursor(0, 1)
        lcd.write(initialBatteryLevel_percent)
        lcd.setCursor(3, 1)
        lcd.write("%")

    print("Sufficient renewbles")
    if userDemand == 0:  # No demand
        while True:
            if computeWaterLevel() > upperWaterLevelThreshold:
                GPIO.output(pumpPin, GPIO.LOW)  # Turn off pump
                print("Water level = {} cm. High.\nPump off. Conveyor stop. No demand." .format(computeWaterLevel()))
            else:
                GPIO.output(pumpPin, GPIO.HIGH)  # Turn on pump
                print("Water level = {} cm. Medium/low.\nPump off. Conveyor stop. No demand." .format(computeWaterLevel()))
            displayBatteryLevel_charging()

    elif userDemand == 1:  # Low demand
        while True:
            if computeWaterLevel() > upperWaterLevelThreshold:
                GPIO.output(pumpPin, GPIO.LOW)  # Turn off pump
                conveyorBelt.runSlow()
                print("Water level = {} cm. High.\nPump off. Conveyor slow. Low demand." .format(computeWaterLevel()))
            elif lowerWaterLevelThreshold <= computeWaterLevel() <= upperWaterLevelThreshold
                GPIO.output(pumpPin, GPIO.HIGH)  # Turn on pump
                conveyorBelt.runSlow()
                print("Water level = {} cm. Medium/low.\nPump on. Conveyor slow. Low demand." .format(computeWaterLevel()))
            elif lowerWaterLevelThreshold > computeWaterLevel()
                GPIO.output(pumpPin, GPIO.HIGH)  # Turn on pump
                conveyorBelt.stop()
                print("Water level = {} cm. Low.\nPump on. Conveyor stop. Low demand." .format(computeWaterLevel()))
            displayBatteryLevel_charging()

    elif userDemand == 2:  # High demand
        while True
            if computeWaterLevel() > upperWaterLevelThreshold:
                GPIO.output(pumpPin, GPIO.LOW)  # Turn off pump
                conveyorBelt.runFast()
                print("Water level = {} cm. High.\nPump off. Conveyor fast. High demand." .format(computeWaterLevel()))
            elif lowerWaterLevelThreshold <= computeWaterLevel() <= upperWaterLevelThreshold
                GPIO.output(pumpPin, GPIO.HIGH)  # Turn on pump
                conveyorBelt.runFast()
                print("Water level = {} cm. Medium/low.\nPump on. Conveyor slow. High demand." .format(computeWaterLevel()))
            elif lowerWaterLevelThreshold > computeWaterLevel()
                GPIO.output(pumpPin, GPIO.HIGH)  # Turn on pump
                conveyorBelt.stop()
                print("Water level = {} cm. Low.\nPump on. Conveyor stop. High demand." .format(computeWaterLevel()))
            displayBatteryLevel_charging()


else:
    lcd.write("Discharging")
    lcd.setCursor(0, 1)
    lcd.write(initialBatteryLevel_percent)
    lcd.setCursor(3, 1)
    lcd.write("%")

    print("Insufficient renewables")
    if userDemand == 0:  # No demand. Conveyor does not move
        while True:
            if computeWaterLevel() >= lowerWaterLevelThreshold:
                if displayBatteryLevel_discharging() > 0:
                    GPIO.output(pumpPin, GPIO.LOW)  # Turn off pump
                    print("Water level = {} cm. Medium/high.\nPump off. Conveyor stop. No demand.\nDischarging from =" .format(computeWaterLevel()))
                else:
                    stopEverything()

            else:
                if displayBatteryLevel_discharging() > 0:
                    GPIO.output(pumpPin, GPIO.HIGH)  # Turn on pump
                    print("Water level = {} cm. Medium/high.\nPump off. Conveyor stop. No demand.\nDischarging from =" .format(computeWaterLevel()))
                else:
                    stopEverything()


    elif userDemand == 1:  # Low demand
        while True:
            if computeWaterLevel() >= lowerWaterLevelThreshold:  # High or medium water level
                conveyorBelt.runSlow()
                if displayBatteryLevel_discharging() > 0:
                    GPIO.output(pumpPin, GPIO.LOW)  # Turn off pump
                    print("Water level = {} cm. Medium/high.\nPump off. Conveyor slow. Low demand.\nDischarging from =" .format(computeWaterLevel()))
                else:
                    stopEverything()

            else:
                conveyorBelt.stop()
                if displayBatteryLevel_discharging() > 0:
                    GPIO.output(pumpPin, GPIO.HIGH)  # Turn on pump
                    print("Water level = {} cm. Low.\nPump on. Conveyor slow. Low demand.\nDischarging from =" .format(computeWaterLevel()))
                else:
                    stopEverything()


    elif userDemand == 2:  # High demand
        while True:
            if computeWaterLevel() >= lowerWaterLevelThreshold:
                conveyorBelt.runFast()
                if displayBatteryLevel_discharging() > 0:
                    GPIO.output(pumpPin, GPIO.LOW)  # Turn off pump
                    print("Water level = {} cm. Medium/high.\nPump off. Conveyor fast. High demand.\nDischarging from =" .format(computeWaterLevel()))
                else:
                    stopEverything()

            else:
                conveyorBelt.stop()
                if displayBatteryLevel_discharging() > 0:
                    GPIO.output(pumpPin, GPIO.HIGH)  # Turn on pump
                    print("Water level = {} cm. Low.\nPump on. Conveyor stop. High demand.\nDischarging from =" .format(computeWaterLevel()))
                else:
                    stopEverything()
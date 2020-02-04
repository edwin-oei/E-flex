import time
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards


# Water level sensor
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

GPIO.cleanup()
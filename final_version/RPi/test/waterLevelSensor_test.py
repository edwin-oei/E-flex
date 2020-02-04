import time
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards


# Water level sensor
trigPin = 23
echoPin = 24

GPIO.setup(trigPin, GPIO.OUT)
GPIO.output(trigPin, 0)
GPIO.setup(echoPin, GPIO.IN)
time.sleep(0.5)  # Let sensors settle

# The HC-SR04 sensor requires a short 10uS pulse to trigger the module, which will cause the sensor to start the ranging program
# (8 ultrasound bursts at 40 kHz) in order to obtain an echo response.
# So, to create our trigger pulse, we set out trigger pin high for 10uS then set it low again.
GPIO.output(trigPin, 1)
time.sleep(0.00001)
GPIO.output(trigPin, 0)

while GPIO.input(echoPin) == 0:
    pass
starTime = time.time()

while GPIO.input(echoPin) == 1:
    pass
endTime = time.time()

pulseDuration = endTime - starTime
waterLevel = pulseDuration*17150

GPIO.cleanup()
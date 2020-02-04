# import final_python
# from final_python import trigPin, echoPin
# import time
# import RPi.GPIO as GPIO
#
# GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards

def computeWaterLevel():
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
    waterLevel = 14 - pulseDuration*17150  # In cm

    return waterLevel
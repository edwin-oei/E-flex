# import final_python
# from final_python import motor_1a, motor_1b, valvePin, pwm, mcp
# import time
# import RPi.GPIO as GPIO
#
# GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards

class conveyorBelt():
    def stop(self):
        GPIO.output(motor_1a, GPIO.LOW)
        GPIO.output(motor_1b, GPIO.LOW)


    def detectObject(self):
        if mcp.read_adc(0) < 100:
            self.stop()
            GPIO.output(valvePin, GPIO.HIGH)  # Valve open
            time.sleep(0.5)  # Duration of open valve
            GPIO.output(valvePin, GPIO.LOW)  # Valve close
            time.sleep(2)  # Buffer time before the conveyor moves again
        else:
            pass


    def runSlow(self):
        pwm.ChangeDutyCycle(50)
        GPIO.output(motor_1a, GPIO.HIGH)
        GPIO.output(motor_1b, GPIO.LOW)
        time.sleep(0.725)
        self.detectObject()


    def runFast(self):
        pwm.ChangeDutyCycle(100)
        GPIO.output(motor_1a, GPIO.HIGH)
        GPIO.output(motor_1b, GPIO.LOW)
        time.sleep(0.85)
        self.detectObject()



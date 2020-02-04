import time
import RPi.GPIO as GPIO
import Adafruit_GPIO.SPI as SPI
import Adafruit_MCP3008

GPIO.setmode(GPIO.BCM)  # BCM = GPIO pin numbering based on broadcom standards
CLK = 6
MISO = 13
MOSI = 19
CS = 26
mcp = Adafruit_MCP3008.MCP3008(clk=CLK, cs=CS, miso=MISO, mosi=MOSI)
values = mcp.read_adc(0)
print(values)
GPIO.cleanup()
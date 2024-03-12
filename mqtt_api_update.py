import RPi.GPIO as GPIO
import os
import requests

def setupGPIO():
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)

    GPIO.setup(18, GPIO.OUT) #relay control
    GPIO.setup(17, GPIO.IN) #FSR Sensor input


def printTemperature():
    temp = os.popen("/opt/vc/bin/vcgencmd measure_temp").read()
    print("GPU temperature is {}".format(temp[5:]))

def readFSRSensor():
    return GPIO.input(17)

def readRelay():
    return GPIO.input(18)

def controlLED():
   try:
        while True:
            object_state = readFSRSensor()
            door_state = readRelay()
            user_input = input("Turn LED On or Off with 1 or 0 (Ctrl-C to exit): ")

            if user_input == "1" and door_state == GPIO.LOW and object_state == GPIO.LOW:
                GPIO.output(18, GPIO.HIGH) #off the magnetic
                print("Door is open! Sending...")
                while object_state == GPIO.LOW: 
                    time.sleep(1)
                else:
                    GPIO.output(18, GPIO.LOW)
                
            else:
                GPIO.output(18, GPIO.LOW)
                print("Door is open! Receiving...")
                while object_state == GPIO.HIGH: 
                    time.sleep(1)
                else:
                    GPIO.output(18, GPIO.LOW)

            time.sleep(1)  # Add a delay to avoid excessive readings
    except KeyboardInterrupt:
        GPIO.cleanup()
        print("")

def print_http_method(api_url):
    try:
        response = requests.get(api_url)
        http_method = response.request.method
        if http_method == "GET":
            print("Request Method is GET")
        elif http_method == "PUT":
            print("Request Method is PUT")
        # Add more conditions as needed for other HTTP methods
        else:
            print("Unknown Request Method")
    except Exception as e:
        print(f"An error occurred: {e}")

def main():
    # Specify the API URL
    api_url = 'https://api.captechvn.com/api/v1/locker'
    
    # Print the HTTP method
    print_http_method(api_url)

    # Split the API URL into an array
    url_parts = api_url.split('//')[-1].split('/')
    
    # Print the array result
    print("URL parts:", url_parts)

if __name__ == "__main__":
    main()
    setupGPIO()
    printTemperature()
    controlLED()

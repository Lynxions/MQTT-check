import time
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

def readRelay(): #su dung thuong dong => luon cap dien cho nam cham. Cap dien relay => ngung cap dien nam cham
    #Relay (0) => Magnetic (1)
    #Relay (1) => Magnetic (0)
    return GPIO.input(18) 

# def controlLED():
#    try:
#         while True:
#             object_state = readFSRSensor()
#             door_state = readRelay()
#             user_input = input("Turn LED On or Off with 1 or 0 (Ctrl-C to exit): ")

#             if user_input == "1" and door_state == GPIO.LOW and object_state == GPIO.LOW:
#                 GPIO.output(18, GPIO.HIGH) #Relay (1) => Magnetic (0)
#                 print("Door is open! Sending...")
#                 while object_state == GPIO.LOW: #Wait to put the object
#                     time.sleep(1)
#                 else:
#                     GPIO.output(18, GPIO.LOW) #Turn Magnetic on when Have OBJECT and close the door by hand
                
#             else:
#                 GPIO.output(18, GPIO.HIGH) #Open the door
#                 print("Door is open! Receiving...")
#                 while object_state == GPIO.HIGH: #Wait to take the object
#                     time.sleep(1)
#                 else:
#                     GPIO.output(18, GPIO.LOW)

#             time.sleep(1)  # Add a delay to avoid excessive readings
#     except KeyboardInterrupt:
#         GPIO.cleanup()
#         print("")

def lockerChecking_Act(http_method):
    object_state = readFSRSensor()
    door_state = readRelay()
    #Check if the locker is avaiable for sending
    if http_method == "GET" and door_state == GPIO.LOW and object_state == GPIO.LOW: #SENDING PACKAGE/GUI HANG + MAGNETIC (ON) + NO OBJECT
        GPIO.output(18, GPIO.HIGH) #Relay (1) => Magnetic (0)
        print("Door is open! Sending the package...")
        while object_state == GPIO.LOW: #Wait to put the object
            time.sleep(1)
        else:
            GPIO.output(18, GPIO.LOW) #Turn Magnetic on when Have OBJECT and close the door by hand

    elif http_method == "PUT" and door_state == GPIO.LOW and object_state == GPIO.HIGH:
        GPIO.output(18, GPIO.HIGH) #Open the door
        print("Door is open! Receiving the package...")
        while object_state == GPIO.HIGH: #Wait to take the object
            time.sleep(1)
        else:
            GPIO.output(18, GPIO.LOW) ##Turn Magnetic on when Have OBJECT and close the door by hand


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

def getHTTP_method(api_url):
    response = requests.get(api_url)
    http_method = response.request.method
    return http_method

def main():
    setupGPIO()
    printTemperature()
    # Specify the API URL
    api_url = 'https://api.captechvn.com/api/v1/locker'
    
    # Print the HTTP method
    print_http_method(api_url)

    # Split the API URL into an array
    url_parts = api_url.split('//')[-1].split('/')
    
    # Print the array result
    print("URL parts:", url_parts)

    lockerChecking_Act(getHTTP_method(api_url))


if __name__ == "__main__":
    main()

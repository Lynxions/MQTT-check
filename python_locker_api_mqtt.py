import uuid
import requests
import paho.mqtt.client as mqtt

class Locker:
    def __init__(self, id, width, height):
        self.id = id
        self.width = width
        self.height = height
        self.is_empty = True
        self.package_id = None

    def is_fit(self, package_width, package_height):
        return self.is_empty and self.width >= package_width and self.height >= package_height

    def set_empty(self, is_empty=True):
        self.is_empty = is_empty

    def get_empty(self):
        return self.is_empty

class LockerSystem:
    def __init__(self):
        self.lockers = []

    def add_locker(self, locker):
        self.lockers.append(locker)

    def find_locker(self, locker_id):
        for locker in self.lockers:
            if locker.id == locker_id:
                return locker
        return None

    def smallest_difference_locker(self, package_width, package_height):
        eligible_lockers = [locker for locker in self.lockers if locker.is_fit(package_width, package_height)]
        return min(eligible_lockers, key=lambda locker: locker.width - package_width + locker.height - package_height, default=None)

    def send_package(self, package_width, package_height):
        locker = self.smallest_difference_locker(package_width, package_height)
        if locker:
            locker.set_empty(is_empty=False)
            package_id = str(uuid.uuid4())
            locker.package_id = package_id
            EspPublish(f"rpi/locker{locker.id}", "on")
            print(f"The package was sent. Locker ID: {locker.id}, Package ID: {package_id}")
        else:
            print("No suitable locker available for this package size.")

    def take_package(self, locker_id):
        locker = self.find_locker(locker_id)
        if locker and not locker.is_empty:
            package_id = input("Enter package ID to take the package: ")
            if package_id == locker.package_id:
                locker.set_empty()
                locker.package_id = None
                EspPublish(f"rpi/locker{locker.id}", "on")
                print("Package taken successfully.")
            else:
                print("Wrong package ID.")
        else:
            print("Invalid locker ID or the locker is empty.")

def EspPublish(topic, message):
    broker = "10.238.55.140"  # MQTT broker address
    port = 1883  # Default MQTT port

    client = mqtt.Client("PythonClient")
    client.connect(broker, port)
    client.publish(topic, message)
    client.disconnect()

def fetch_locker_status(api_url):
    response = requests.get(api_url)
    return response.status_code == 401  # Assuming 200 indicates a successful response

def extract_number_from_url(url):
    parts = url.split('/')
    try:
        return int(parts[-1])
    except (ValueError, IndexError):
        return 0

def main():
    locker_system = LockerSystem()
    locker_system.add_locker(Locker(id=1, width=30, height=30))
    locker_system.add_locker(Locker(id=2, width=10, height=20))
    locker_system.add_locker(Locker(id=3, width=50, height=45))
    locker_system.add_locker(Locker(id=4, width=5, height=20))

    while True:
        url = input("Enter '0' to send a package or enter a locker ID to take a package (or 'exit' to quit): ")
        if url.lower() == 'exit':
            break

        if fetch_locker_status(url):
            locker_id = int(extract_number_from_url(url))
            if locker_id == 0:
                package_height = int(input("Enter the package height: "))
                package_width = int(input("Enter the package width: "))
                locker_system.send_package(package_width, package_height)
            else:
                locker_system.take_package(locker_id)
        else:
            print("Failed to fetch locker status from the API.")

if __name__ == "__main__":
    main()

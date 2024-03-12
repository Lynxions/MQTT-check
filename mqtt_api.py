import requests

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

if __name__ == "__main__":
    main()

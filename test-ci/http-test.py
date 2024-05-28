import http.client
import os

success = True

PROJ_ROOT_DIR = os.path.abspath(os.path.dirname(__file__) + "/../")

GREEN = "\033[92m"
RED = "\033[91m"
ENDC = "\033[0m"

def cmp(testName, actual, expected):
		global success
		if actual == expected:
				print(f"Test {testName} {GREEN}passed{ENDC}")
		if actual != expected:
				print(f"Test {testName} {RED}failed{ENDC}: expected {expected}, got {actual}")
				success = False

host = "127.0.0.1"

def Test1():
	conn = http.client.HTTPConnection(host)
	conn.request("GET", "/resources/sh-cgi/document.sh", headers={"Host": host})
	response = conn.getresponse()

	expectedBody = f"""\
Gateway Interface: CGI/1.1
Path Info:{' '}
Path Translated: {PROJ_ROOT_DIR}/resources/sh-cgi/document.sh
Query String:{' '}
Request Method: GET
Script Name: /resources/sh-cgi/document.sh
Remote Address: 127.0.0.1
Server Name: localhost
Server Port: 80
Server Protocol: HTTP/1.1
Server Software: webserv/1.0
Hello, World!
"""

	cmp("status", response.status, 200)
	cmp("reason", response.reason, "OK")
	headers = dict(response.getheaders())
	cmp("header count", len(headers), 3)
	cmp("content-type", headers["Content-Type"], "text/plain")
	cmp("content-length", headers["Content-Length"], str(len(expectedBody)))
	cmp("server", headers["Date"] != "", True)

	actualBody = response.read().decode()
	cmp("body-len", len(actualBody), len(expectedBody))
	cmp("body", actualBody, expectedBody)

	conn.close()


print("Running tests...")

print("Test1")
Test1()

if success:
		print(f"{GREEN}All tests passed{ENDC}")
else:
		print(f"{RED}Some tests failed{ENDC}")
		exit(1)

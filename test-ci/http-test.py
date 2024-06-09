import http.client
import os
import re
from typing import Callable

success = True

PROJ_ROOT_DIR = os.path.abspath(os.path.dirname(__file__) + "/../")

GREEN = "\033[92m"
RED = "\033[91m"
ENDC = "\033[0m"

def cmp_func(testName: str, actual: str, func: Callable[[str], bool]):
		global success
		if func(actual):
				print(f"Test {testName} {GREEN}passed{ENDC}")
		else:
				print(f"Test {testName} {RED}failed{ENDC}: got {actual}")
				success = False
def cmp_re(testName: str, actual: str, pattern: str | re.Pattern[str]):
		if isinstance(pattern, str):
				cmp_func(testName, actual, re.compile(pattern).fullmatch)
		else:
				cmp_func(testName, actual, pattern.fullmatch)
def cmp(testName: str, actual, expected: str | re.Pattern[str] | int | bool | Callable[[str], bool]):
		global success
		if isinstance(expected, re.Pattern):
				cmp_re(testName, actual, expected)
		elif callable(expected):
				cmp_func(testName, actual, expected)
		elif actual == expected:
				print(f"Test {testName} {GREEN}passed{ENDC}")
		else:
				print(f"Test {testName} {RED}failed{ENDC}: expected {expected}, got {actual}")
				success = False
def cmp_list(testName: str, actual: list[str], expected: list[str | re.Pattern[str] | Callable[[str], bool]]):
		cmp(f"{testName}_length", len(actual), len(expected))
		for i in range(max(len(actual), len(expected))):
				actualValue = actual[i] if i < len(actual) else None
				expectedValue = expected[i] if i < len(expected) else None
				cmp(f"{testName}[{i}]", actualValue, expectedValue)

host = "localhost"

def Test1():
	conn = http.client.HTTPConnection(host)
	conn.request(
		"GET",
		"/resources/sh-cgi/document.sh",
		headers={
			"Host": host,
			"Accept-Encoding": "identity",
		}
	)
	response = conn.getresponse()

	cmp("status", response.status, 200)
	cmp("reason", response.reason, "OK")
	headers = dict(response.getheaders())
	cmp("header count", len(headers), 4)
	cmp("content-type", headers["Content-Type"], "text/plain")
	cmp("content-length", headers["Content-Length"], lambda actual: actual.isdigit() and int(actual) > 0)
	cmp("server", headers["Date"] != "", True)

	actualBody = response.read().decode()
	cmp_list(
		"body",
		actualBody.split("\n"),
		[
			"Content-Type: ",
			"Content-Length: ",
			"Gateway Interface: CGI/1.1",
			"Path Info: ",
			f"Path Translated: {PROJ_ROOT_DIR}/resources/sh-cgi/document.sh",
			"Query String: ",
			"Request Method: GET",
			"Script Name: /resources/sh-cgi/document.sh",
			"Remote Address: 127.0.0.1",
			lambda actual: actual.startswith("Remote Port: ") and actual[14:].isdigit() and int(actual[14:]) > 0 and int(actual[14:]) < 65536,
			f"Server Name: {host}",
			"Server Port: 80",
			"Server Protocol: HTTP/1.1",
			"Server Software: webserv/1.0",
			"Hello, World!",
			"",
			"HTTP_ACCEPT_ENCODING=identity",
			f"HTTP_HOST={host}",
			"",
			"",
		]
	)

	conn.close()

def TestPathInfo():
	conn = http.client.HTTPConnection(host)
	conn.request(
		"GET",
		"/resources/sh-cgi/document.sh/abc/def?query=value&key=value2",
		headers={
			"Host": host,
			"Accept-Encoding": "identity",
		}
	)
	response = conn.getresponse()

	cmp("status", response.status, 200)
	cmp("reason", response.reason, "OK")
	headers = dict(response.getheaders())
	cmp("header count", len(headers), 4)
	cmp("content-type", headers["Content-Type"], "text/plain")
	cmp("content-length", headers["Content-Length"], lambda actual: actual.isdigit() and int(actual) > 0)
	cmp("server", headers["Date"] != "", True)

	actualBody = response.read().decode()
	cmp_list(
		"body",
		actualBody.split("\n"),
		[
			"Content-Type: ",
			"Content-Length: ",
			"Gateway Interface: CGI/1.1",
			"Path Info: /abc/def",
			f"Path Translated: {PROJ_ROOT_DIR}/resources/sh-cgi/document.sh",
			"Query String: query=value&key=value2",
			"Request Method: GET",
			"Script Name: /resources/sh-cgi/document.sh",
			"Remote Address: 127.0.0.1",
			lambda actual: actual.startswith("Remote Port: ") and actual[14:].isdigit() and int(actual[14:]) > 0 and int(actual[14:]) < 65536,
			f"Server Name: {host}",
			"Server Port: 80",
			"Server Protocol: HTTP/1.1",
			"Server Software: webserv/1.0",
			"Hello, World!",
			"",
			"HTTP_ACCEPT_ENCODING=identity",
			f"HTTP_HOST={host}",
			"",
			"",
		]
	)

	conn.close()

def TestBody(count = 100):
	BODY_STR_LIST = ["Hello, World!", "from Python"] * count
	BODY_STR = "\n".join(BODY_STR_LIST)
	print(f"Body length: {len(BODY_STR)}")
	conn = http.client.HTTPConnection(host)
	conn.request(
		"POST",
		"/resources/sh-cgi/document.sh/abc/def?query=value&key=value2&chunked=false",
		headers={
			"Host": host,
			"Accept-Encoding": "identity",
			"Content-Type": "text/plain",
		},
		body=BODY_STR,
	)
	response = conn.getresponse()

	cmp("status", response.status, 200)
	cmp("reason", response.reason, "OK")
	headers = dict(response.getheaders())
	cmp("header count", len(headers), 4)
	cmp("content-type", headers["Content-Type"], "text/plain")
	cmp("content-length", headers["Content-Length"], lambda actual: actual.isdigit() and int(actual) > 0)
	cmp("server", headers["Date"] != "", True)

	actualBody = response.read().decode()

	expected = [
		"Content-Type: text/plain",
		f"Content-Length: {len(BODY_STR)}",
		"Gateway Interface: CGI/1.1",
		"Path Info: /abc/def",
		f"Path Translated: {PROJ_ROOT_DIR}/resources/sh-cgi/document.sh",
		"Query String: query=value&key=value2&chunked=false",
		"Request Method: POST",
		"Script Name: /resources/sh-cgi/document.sh",
		"Remote Address: 127.0.0.1",
		lambda actual: actual.startswith("Remote Port: ") and actual[14:].isdigit() and int(actual[14:]) > 0 and int(actual[14:]) < 65536,
		f"Server Name: {host}",
		"Server Port: 80",
		"Server Protocol: HTTP/1.1",
		"Server Software: webserv/1.0",
		"Hello, World!",
		"",
		"HTTP_ACCEPT_ENCODING=identity",
		f"HTTP_HOST={host}",
		"",
	]
	expected.extend(BODY_STR_LIST)

	cmp_list(
		"body",
		actualBody.split("\n"),
		expected
	)

	conn.close()

def TestChunkedBody(count = 100):
	BODY_STR_LIST = ["Hello, World!", "from Python"] * count
	BODY_STR = "\n".join(BODY_STR_LIST)
	print(f"Body length: {len(BODY_STR)}")
	conn = http.client.HTTPConnection(host)
	conn.request(
		"POST",
		"/resources/sh-cgi/document.sh/abc/def?query=value&key=value2&chunked=true",
		headers={
			"Host": host,
			"Accept-Encoding": "identity",
			"Content-Type": "text/plain",
			"Transfer-Encoding": "chunked",
		},
		body=BODY_STR,
		encode_chunked=True,
	)
	response = conn.getresponse()

	cmp("status", response.status, 200)
	cmp("reason", response.reason, "OK")
	headers = dict(response.getheaders())
	cmp("header count", len(headers), 4)
	cmp("content-type", headers["Content-Type"], "text/plain")
	cmp("content-length", headers["Content-Length"], lambda actual: actual.isdigit() and int(actual) > 0)
	cmp("server", headers["Date"] != "", True)

	actualBody = response.read().decode()

	expected = [
		"Content-Type: text/plain",
		f"Content-Length: {len(BODY_STR)}",
		"Gateway Interface: CGI/1.1",
		"Path Info: /abc/def",
		f"Path Translated: {PROJ_ROOT_DIR}/resources/sh-cgi/document.sh",
		"Query String: query=value&key=value2&chunked=true",
		"Request Method: POST",
		"Script Name: /resources/sh-cgi/document.sh",
		"Remote Address: 127.0.0.1",
		lambda actual: actual.startswith("Remote Port: ") and actual[14:].isdigit() and int(actual[14:]) > 0 and int(actual[14:]) < 65536,
		f"Server Name: {host}",
		"Server Port: 80",
		"Server Protocol: HTTP/1.1",
		"Server Software: webserv/1.0",
		"Hello, World!",
		"",
		"HTTP_ACCEPT_ENCODING=identity",
		f"HTTP_HOST={host}",
		"HTTP_TRANSFER_ENCODING=chunked",
		"",
	]
	expected.extend(BODY_STR_LIST)

	cmp_list(
		"body",
		actualBody.split("\n"),
		expected
	)

	conn.close()


print("Running tests...")
print()

print("Test1")
Test1()

print()
print("TestPathInfo")
TestPathInfo()

print()
print("TestBody")
TestBody()

print()
print("TestChunkedBody")
TestChunkedBody()

print()
print("TestBody - Big")
TestBody(5000)

print()
print("TestChunkedBody - Big")
TestChunkedBody(5000)

if "CI" in os.environ:
	print()
	print("TestBody - VeryBig")
	TestBody(500000)

	print()
	print("TestChunkedBody - VeryBig")
	TestChunkedBody(500000)


print()

if success:
		print(f"{GREEN}All tests passed{ENDC}")
else:
		print(f"{RED}Some tests failed{ENDC}")
		exit(1)

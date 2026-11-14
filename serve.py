#!/usr/bin/env python3
import os
import signal
import sys
from http.server import HTTPServer, SimpleHTTPRequestHandler

FIRST_PORT = 8000
PORT_ATTEMPTS = 100
SECONDS_BETWEEN_QUIT_CHECKS = 0.5

send_headers_end = SimpleHTTPRequestHandler.end_headers
def send_headers_end_without_caching(handler):
    handler.send_header("Cache-Control", "no-cache, no-store, must-revalidate")
    send_headers_end(handler)
SimpleHTTPRequestHandler.end_headers = send_headers_end_without_caching

HTTPServer.allow_reuse_address = os.name != "nt"
os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), "build"))

server = None
for port in range(FIRST_PORT, FIRST_PORT + PORT_ATTEMPTS):
    if server is None:
        try:
            server = HTTPServer(("", port), SimpleHTTPRequestHandler)
        except OSError:
            server = None
if server is None:
    sys.exit(f"no free port from {FIRST_PORT} to {FIRST_PORT + PORT_ATTEMPTS - 1}")

should_quit = False
def request_quit(signal_number, frame):
    global should_quit
    should_quit = True
signal.signal(signal.SIGINT, request_quit)

print(f"serving {os.getcwd()} at http://localhost:{server.server_port}/ (Ctrl-C to quit)", flush=True)
server.timeout = SECONDS_BETWEEN_QUIT_CHECKS
while not should_quit:
    server.handle_request()
server.server_close()
print("stopped", flush=True)

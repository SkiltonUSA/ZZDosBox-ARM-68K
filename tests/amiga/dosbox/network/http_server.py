#!/usr/bin/env python3
"""Temporary bounded HTTP fixture; never exposes filesystem contents."""
import argparse,http.server,ipaddress,time
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--bind',required=True);p.add_argument('--peer',required=True);p.add_argument('--port',type=int,default=55127);p.add_argument('--seconds',type=int,default=600);a=p.parse_args()
a.bind=str(ipaddress.IPv4Address(a.bind));a.peer=str(ipaddress.IPv4Address(a.peer))
fixture=bytes(i%251 for i in range(4099))
class Handler(http.server.BaseHTTPRequestHandler):
 def setup(self):
  super().setup();self.connection.settimeout(5)
 def do_GET(self):
  if self.client_address[0]!=a.peer or self.path!='/fixture':self.send_error(403);return
  self.send_response(200);self.send_header('Content-Length',str(len(fixture)));self.end_headers();self.wfile.write(fixture)
 def log_message(self,fmt,*args):print(self.client_address[0],fmt%args,flush=True)
server=http.server.HTTPServer((a.bind,a.port),Handler);server.timeout=1
print('READY HTTP',a.bind,a.port,flush=True);until=time.monotonic()+a.seconds
try:
 while time.monotonic()<until:server.handle_request()
finally:server.server_close()

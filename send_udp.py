# Python code to send stop over UDP

import argparse
import socket as s
from datetime import datetime

parser = argparse.ArgumentParser(description='Sends UDP plain text to a given IP and Port')
default = str(datetime.now().time())
print(default)
parser.add_argument('-m', '--msg', default=str(datetime.now().time()), help='the text message')
parser.add_argument('-p', '--port', type=int, default=44444, help='the destination port')
parser.add_argument('ip', help='the destination IP')

args = parser.parse_args()

sock = s.socket(s.AF_INET, s.SOCK_DGRAM)
mess = bytes(args.msg, "utf-8")
addr = (bytes(args.ip, "utf-8"), args.port)

sock.sendto(mess, addr)

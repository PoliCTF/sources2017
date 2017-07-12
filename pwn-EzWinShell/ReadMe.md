# EzWinShell  

> Today getting a shell is as easy as 1,2,3  
  
TL;DR:    Getting rce by chaining buffer overflow, write-what-where, tls  

  
## Program description  
  
Windows executable 32bit compiled with DEP+GS+ASLR enabled  
The program contains a reverse shell, ready to be used.  
   
- With option 1 you set ip and port  
- With option 2 you connect but still no shell, only tcp connection (note the attacker should be listening on a public ip for this to work)  
- With option 3 you launch the shell (cmd.exe) (Note this option can't actually be used because it checks the owner is the correct one: strcmp "Nesos\0")  
  
In the menu there is a way to set the owner (as hex string)  
Once you do this... you still can't spawn the shell, as the program says that you are not in "debug mode" and the shell is disabled (just a random excuse, internally it check for three different int values and they must be ==1). In order to solve the challenge you need to find a way to set the three checks to 1, then use option 3 to spawn the shell.  
  
The vulnerability is in the SetOwner function: it has an overflow of 4 bytes while converting from the hex string in input to the ascii correspondent (eg "41414100" to "AAA")  
The owner string is 8 bytes, while you can write 12, overflowing by 4 bytes. The string is intentionally very short because i don't want people to abuse this in unintended ways, no rop or whatever; there must be only my bug!  
  
Following the owner string there is a pointer to it (that can be overwritten).  
Since SetOwner function write where that pointer point we gain a write-what-where primitive of 12 bytes.  
  
I have added extra stack randomization and 1 second delay to prevent bruteforce: sub esp,max0xFFFF wasted max 65k of memory.  
Even if you knew where the stack is (and shouldn't be the case), you'd need 4 hours to solve this; 2 hours half "key space"... except if you spawn multiple connections at once, then you're probably faster. I've added these checks hoping to prevent people from bruteforcing the solution, as the intended exploit is 100% reliable.  
  
### How to solve:  
  
Objective: set check1, check2, check3, call `spawn shell` option.  
  
First thing to do is use `PrintShellcode` option; this will print a small piece of the function where the only useful thing is the ASLR leak from the GS/stack cookie initialization:  
  
`text:00401566 mov eax, ___security_cookie`  
  
Since the exe is 32bit and 32bit doesn't support *HighEntropyVA* the exe is randomized as a single block (and not each section separatly).  
With this leak of the *.data* section location we can compute the location of the *ImageBase* and *.text* section.  
  
`ImageBase=leakVA-stackCookieRVA`  
  
There are three functions that sets the "debug mode" one for each check; they are never used, but can be found if you search references to the checks:  
  
-  `.text:00402DF0 sets the data section check (int==1)`  
-  `.text:00402E10 sets the first heap check (pointer to int==1)`  
-  `.text:00402E1F sets the second heap check (pointer to int==1)`  
  
here is the interesting part:  
  
```  
char owner[8] = "";  
char \* ptrOwner = (char*)&owner;//SetOwner write where this points  
int \*debugModeHeap2 = NULL;//malloc at runtime  
int dummy = 0;  
```  
  
Since a check is near the *ptrOwner* pointer, we can overwrite the *ptrOwner* pointer, changing it to point to itself. This way we can still change "where" and also pass one of the three checks. We can solve this either by setting the *debugModeHeap2* pointer so that it points to the data section check or by pointing to the dummy int and setting it to 1. Meanwhile we also set the correct owner "Nesos\0".   
  
Changing the *ptrOwner* pointer is not a problem, as strncmp will use a direct reference to *owner* string.  
  
Our first line of exploit will be (supposing no aslr):  
  
```  
SetOwner("Nesos\x00AA"+p32(ownerPtrVA))  
4E65736F7300414100405020 .data:00405020= 0x00405020  
```  
  
This will overwrite *ptrOwner*, we can still control where the pointer points (the "where" of our write-what-where primitive). We set the pointer so that it points to itself; we now overwrite the first check and also move the pointer to point to the tls callback array:  
  
```  
SetOwner(p32(tlsCallbacksArrayVA)+p32(ownerPtrVA+8)+p32(1))  
445040002450400001000000   
```  
  
From now on, we won't be able to change "where" anymore but we won't need to. As for the other two checks we will make use of *TLS Callbacks!*.  
  
Since many debuggers/disassemblers detect the presence of tls callbacks, I haven't added any, but I have added the *TLS Directory* in the *PE Header*  so they can be used!  
  
  
### Quick intro on tls callbacks:   
  
- There is a TLS Directory in the PE Header  
- There is a NULL-terminated array of callbacks (pointers to functions)  
- Those functions are called *before* main/entry point and on new threads creation  
- They are rarely used and that's a reason to insert them ;)  
- They are also called on process/thread closing: both return 0; and ExitProcess(0); and this was the biggest "secret", almost noone knew this ;)  
- Malware sometimes use them: a notable case is UPX packer where upx -d[ecompress] decoded a normal exe but doubleclicking on the original launched a malware: tls was used to patch the unpacking routine at runtime.  
- I have read that delphi uses them for it's internal purpose (never checked)  
- they are called by *ntdll*  
- callbacks can be added/edited at runtime by changing the array  
- the callback prototype is: `void _stdcall TlsCallback(PVOID DllHandle, DWORD dwReason, PVOID __formal) `  
  
This might let you think that we can't use them to set the remaining two checks and SpawnShell, after all we don't have stdcall functions with three parameters:  
  
- Setchecks are *stdcall* with only one (dummy) parameter  
- SpawnShell is *cdecl* with no parameters  
  
Luckily *ntdll* will fix the stack for us just after the call, by `mov esp,esi`. So we don't need to have a correct function, almost any function will work (as soon as the function doesn't write too much on the stack).  
  
With SetOwner we can thus change the pointer and point it to the tlsCallbacksArray.  
  
We will set the three callbacks to:  
  
```  
.text:00402DF0 data check  
.text:00402E10 heap check  
.text:00401560 SpawnShell  
```  
  
When we exit, they will be called in this order and *cmd* will be launched just before the program exit. Launching is set so that child will inherit handles and has stdio redirected, in this way also if main process is ended, the connection started by it will survive.  
  
### Exploits that will not work  
  
- Setting all three checks by using the write-what-where primitive: after setting two out of three you will not be able to change where.  
You *Can't* change all three checks  because they are intentionally "far" and you can overwrite only 12 consecutive bytes  
- Setting the TLS array to set all three checks: since tls is triggered on close you will have a program that pass the checks... but a closed program is useless.  
- A ROP: you don't know where stack is and also if you somehow know, you can't do this because you don't know where WinExec is:  
```
setowner1: <cmd\0><dummy4bytes><setowner stack return address>  
setowner2: <winexec><return addr of winexec=main><ourcmd above><uCmdShow>  
```  
(uCmdShow can't be set by rop how we want but we don't care)  
- Using a single TLS that point in the middle of SpawnShell (so that we skip checks): i set the program to be opened (*cmd*) before checks and open it after checks; skipping them you will skip also the string initialization so you will spawn nothing  
  
If i'm right you can't do rop (return oriented programming) because there are only 12 bytes (3 addresses) and the above functions are stdcall with a dummy parameter; also there shouldn't be a stack leak.  
Aother rop idea might be: return to one set check function; it has a pop of a parameter so you are left with only one address, the best you can do is probably to return a bit after main start (so that checks are not cleared) and do this N times. not sure it will work because you can't change anymore "where" of our primitive.  
  

  
### Difficulty  
  
I have no idea... if you aren't used to mess with Windows internals it's quite hard, half-undocumented thigs and also rarely used, ntdll instruction, things that happens on closing... all this make me thing that is hard.  
On the other side the exploit it kinda simple:  
- press 4=leak -> aslr gone  
- x3 set owner  
- press 1 & 2: set ip & connect  
- exit  
  
### Known bugs  
- Wine doesn't support ASLR '-_- i hoped noone noticed it, i was wrong. hoped also it was not a problem because i actually leak aslr intentionally.  
(but it support dep and page protections so that text is not writable)  
- After noticing that wine has no aslr i have added stack randomization but i forgot to replace return -1 with exit process -1 (it crash accessing 0x0); not a real problem because it never fail to malloc 4 bytes and you can't do anything with it.  
- Reported sha256 was incorrect, anyway the challenge was correct and has neven been replaced during the ctf.  
  
### Unknown bugs  
- Wine crash dumps were enabled so you had *almost anything!!!* DOH!!. I have not deployed it on the remote server but i admit that i forgot this... i have extensively tested on windows, much less on wine. We have then disabled crash dumps (there were still 0 solves).  
- Thought/hoped that dll were randomized on wine: exe randomization is an extra (aslr) but dll relocation should be by design, if more than one has same imagebase, seems i was wrong or maybe this is related to crashdump problem.  
- Processes (unlike on windows) were not always stopped so we ended up with many instances of the exe running and things got slow, we added a killall each n minutes.  
- There is a unintended solution involving overwriting strncmp@got inside msvcrt.dll.so. I hope someone will write a writeup on this! I tried hard to avoid unintended solutions but as expected people have mad skills xD  
  
### Conclusion  
  
I wanted to make something hard as people playing ctfs are very skilled, something without guessing/long bruteforce and also something different and new, not "the usual rop".  
Judging from the comments I'd say mission accomplished ;)  
  
- oh, wow, never heard about that :) thank you!, great stuff!  
- oh fuck  
- oh damn, i didnt know that :/ thanks very much!  
- but aren't those called at program startup?  
- solved it with the callbacks, was a cool idea!  
- argh, i also thought tls was only on startup :'(  
  
Thanks anyone for playing!  
Greetings from Italy  
Nesos  

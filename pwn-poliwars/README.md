# Poli Wars

A not so long time ago, in a galaxy not so far away.. 

## How to build

   gcc -m32 -o poli_wars poli_wars.c ; strip poli_wars

## How to run   
   
   socat tcp-l:some_port,reuseaddr,fork exec:"./poli_wars"
   
## How to solve

   *** SPOILER ALERT ***
   
   See `pwnpoli_wars`

In a nutshell:

1. House of Mind heap exploitation technique ([Reference](http://phrack.org/issues/66/10.html) - FastBin Method) is used to set global variable which controls falcon deployment to a value != 0. Bwings are used for padding heap to correct position. Xwing to overwrite top chunk size.
2. The particular ship_name of the z95 fighter (z95;) allow us to preapare a structure with z95;/bin/sh\x00 as first 3 params to bee freed later.
3. Falcon allow us to leak all we need (got and libc addresses).
4. Falcon is also vulnerable to heap overflow with no check thus allowing us to use House of Force technique.
5. 'Houseforcing' into got allow us to replace free address with system.
6. Sending our z95 spacecraft alone to face deathstar results in shell.

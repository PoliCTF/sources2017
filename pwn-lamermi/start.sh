java \
 -Djava.security.policy=security.policy \
 -Djava.rmi.server.useCodebaseOnly=false \
 -Djava.rmi.server.hostname=lamermi.chall.polictf.it \
 -jar lamermi-1.0-SNAPSHOT.jar

#cd codebase
#python2 -m SimpleHTTPServer 8000 &

echo "server started"
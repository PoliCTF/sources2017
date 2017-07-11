#!/bin/bash
RED='\033[0;31m'
NC='\033[0m' # No Color
cd src
printf "${RED}[*] Compiling level1${NC}\n"
cd server_udp_level1
make
printf "${RED}[*] Compiling level2${NC}\n"
cd ..
cd server_udp_level2
make

cd ..

printf "${RED}[*] Compiling server_fork${NC}\n"

cd server_fork
make

printf "${RED}[*] Done${NC}\n"
printf "${RED}[*] Creating \"deployed\" folder${NC}\n"

chmod +x server_fork
chmod +x level1/level1
chmod +x level2/level2

cd ..
cd ..
rm -rf deployed
rm -rf to_give
cp -rf src/server_fork deployed
rm deployed/server_fork.c
rm deployed/makefile

printf "${RED}[*] Creating \"to_give\" folder${NC}\n"
cp -rf deployed/ to_give/
cd to_give
rm level1/flag
rm level2/flag

cp -rf ../pong_client ./pong_client
cd pong_client
rm -rf project


printf "${RED}[*] Server build done${NC}\n"

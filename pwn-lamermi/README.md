# LameRMI challenge for Polictf2017

## Solutions

- Refer to `writeup.md` for a writeup
- author's solution is in `src/client`
- mortic's solution is in `solution-mortic`

(move the exploit classes to a public codebase and edit hardcoded url in exploits accordingly)

## Deploy requirements

* JRE 8
* any webserver (the one bundled with python is fine)
* please set permissions to the folder such that only the flag can be read
* firewall must allow outgoing http connections

## Deploying

- run `gradle genPackage`
- deploy the generated `build/package` folder to a server
- fix `the java.rmi.server.hostname in start.sh script`
- run `bash start.sh`
- serve the `codebase` folder with any http server
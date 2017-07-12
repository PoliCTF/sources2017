package it.polictf.lamermi;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.List;

/**
 * Created by vigliag on 2/13/16.
 */
public interface AverageService extends Remote {
    Double average(List<Integer> integerList) throws RemoteException;
}

package it.polictf.lamermi;

import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.List;

/**
 * Created by vigliag on 2/13/16.
 */
public class AverageImpl extends UnicastRemoteObject implements AverageService {

    protected AverageImpl() throws RemoteException {
        super();
    }

    @Override
    public Double average(List<Integer> integerList) throws RemoteException {
        int sum = 0;
        int count = 0;
        for (Integer i : integerList) {
            sum+= i;
            count++;
        }
        if(count != 0){
            return ((double)sum)/count;
        } else {
            return (double)count;
        }
    }
}

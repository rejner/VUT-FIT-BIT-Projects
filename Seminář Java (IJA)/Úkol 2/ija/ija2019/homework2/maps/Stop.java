package ija.ija2019.homework2.maps;

import ija.ija2019.homework2.interfaces.*;
import java.util.Objects;

public class Stop implements IStop {

    private String id;
    private Coordinate position;
    private IStreet s;

    public Stop(String id, Coordinate position) {
        this.id = id;
        this.position = position;
    }

    public static Stop defaultStop(String id, Coordinate c){
        Stop stop = new Stop(id, c);
        return stop;
    }

    // constructor overloading
    public Stop(String id) {
        this.id = id;
        this.position = null;
    }

    @Override
    public String getId(){
        return this.id;
    }

    @Override
    public Coordinate getCoordinate() {
        return this.position;
    }

    @Override
    public IStreet getStreet() {
        return (this.s != null ? this.s : null);
    }

    @Override
    public void setStreet(IStreet s) {
        this.s = s;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Stop Stop = (Stop) o;
        return Objects.equals(id, Stop.id);
    }

    @Override
    public String toString() {
        return "stop(" + getId() + ")";
    }
}

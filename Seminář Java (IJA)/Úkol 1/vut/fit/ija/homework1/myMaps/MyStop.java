package vut.fit.ija.homework1.myMaps;

import vut.fit.ija.homework1.maps.Coordinate;
import vut.fit.ija.homework1.maps.Stop;
import vut.fit.ija.homework1.maps.Street;

import java.util.Objects;

public class MyStop implements Stop {

    private String id;
    private Coordinate position;
    private Street s;

    public MyStop(String id, Coordinate position) {
        this.id = id;
        this.position = position;
    }

    // constructor overloading
    public MyStop(String id) {
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
    public Street getStreet() {
        return (this.s != null ? this.s : null);
    }

    @Override
    public void setStreet(Street s) {
        this.s = s;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        MyStop myStop = (MyStop) o;
        return Objects.equals(id, myStop.id);
    }
}

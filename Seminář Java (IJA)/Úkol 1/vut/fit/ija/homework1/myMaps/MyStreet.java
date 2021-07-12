package vut.fit.ija.homework1.myMaps;

import vut.fit.ija.homework1.maps.Coordinate;
import vut.fit.ija.homework1.maps.Stop;
import vut.fit.ija.homework1.maps.Street;

import java.util.ArrayList;
import java.util.List;

public class MyStreet implements Street {

    private String id;
    private List<Coordinate> coordinatesList;
    private List<Stop> stopsList;


    public MyStreet(String id, Coordinate c1, Coordinate c2) {
        this.id = id;
        this.coordinatesList = new ArrayList<Coordinate>();
        this.stopsList = new ArrayList<Stop>();
        this.coordinatesList.add(c1);
        this.coordinatesList.add(c2);
    }

    @Override
    public String getId(){
        return this.id;
    }

    @Override
    public List<Coordinate> getCoordinates(){
        return this.coordinatesList;
    }

    @Override
    public List<Stop> getStops() {
        return this.stopsList;
    }

    @Override
    public void addStop(Stop stop) {
        this.stopsList.add(stop);
        stop.setStreet(this);
    }
}

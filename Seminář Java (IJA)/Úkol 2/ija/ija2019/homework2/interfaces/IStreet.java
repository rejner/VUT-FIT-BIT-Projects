package ija.ija2019.homework2.interfaces;
import ija.ija2019.homework2.maps.Coordinate;

import java.util.ArrayList;
import java.util.List;

public interface IStreet {

    public String getId();

    public List<Coordinate> getCoordinates();

    public List<IStop> getStops();

    public boolean addStop(IStop stop);

    public Coordinate begin();

    public Coordinate end();

    public boolean follows(IStreet s);
}

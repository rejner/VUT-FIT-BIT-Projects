package ija.ija2019.homework2.interfaces;
import ija.ija2019.homework2.maps.Coordinate;

import java.util.Objects;

public interface IStop {

    public String getId();

    public Coordinate getCoordinate();

    public IStreet getStreet();

    public void setStreet(IStreet s);

    @Override
    public boolean equals(Object o);

    @Override
    public String toString();
}

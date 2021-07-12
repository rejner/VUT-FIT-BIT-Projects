package ija.ija2019.homework2.interfaces;

import java.util.AbstractMap.SimpleImmutableEntry;
import java.util.ArrayList;
import java.util.List;

public interface ILine {

    public boolean addStop(IStop stop);

    public boolean addStreet(IStreet street);

    public List<SimpleImmutableEntry<IStreet, IStop>> getRoute();

}

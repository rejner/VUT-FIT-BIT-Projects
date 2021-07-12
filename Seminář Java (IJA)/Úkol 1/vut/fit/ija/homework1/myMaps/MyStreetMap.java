package vut.fit.ija.homework1.myMaps;

import vut.fit.ija.homework1.maps.Street;
import vut.fit.ija.homework1.maps.StreetMap;

import java.util.ArrayList;
import java.util.List;

public class MyStreetMap implements StreetMap {

    private List<Street> streetList;

    public MyStreetMap(){
        this.streetList = new ArrayList<Street>();
    }

    @Override
    public void addStreet(Street s) {
        this.streetList.add(s);
    }

    @Override
    public Street getStreet(String id) {

        for (Street str : this.streetList) {
            if (str.getId().equals(id)) {
                return str;
            }
        }
        return null;
    }

}

package ija.ija2019.homework2.maps;

import ija.ija2019.homework2.interfaces.*;

import java.util.ArrayList;
import java.util.List;
import java.util.AbstractMap.SimpleImmutableEntry;

public class Line implements ILine {

    private String id;
    private List<IStop> stopList;
    private List<IStreet> streetList;

    public static Line defaultLine(String id){
        Line line = new Line();
        line.id = id;
        line.stopList = new ArrayList<IStop>();
        line.streetList = new ArrayList<IStreet>();
        return line;
    }

    @Override
    public boolean addStop(IStop stop){

        if (stopList.isEmpty()){
            stopList.add(stop);
            streetList.add(stop.getStreet());
            return true;
        }

        if (streetList.get(streetList.size() - 1).getId() != stop.getStreet().getId()){

            // if street doesn't follow lastly inserted street, return false, else add street into the streetList
            if (! streetList.get(streetList.size() - 1).follows(stop.getStreet())){
                return false;
            } else {
                streetList.add(stop.getStreet());
                stopList.add(stop);
            }
        } else {
            stopList.add(stop);
        }

        return true;
    }

    @Override
    public boolean addStreet(IStreet street){

        // can't insert street when line has no stops yet
        if (stopList.isEmpty()){
            return false;
        }

        // if street follows lastly inserted street, add street to the streetList
        if (street.follows(streetList.get(streetList.size() - 1))){
            streetList.add(street);
        }
        return true;
    }

    @Override
    public List<SimpleImmutableEntry<IStreet, IStop>> getRoute(){

        List<SimpleImmutableEntry<IStreet, IStop>> routeList = new ArrayList<>();

        for (IStreet str : streetList){

            Boolean atLeastOne = false;
            for (IStop stop : stopList){
                if (str.getId() == stop.getStreet().getId()){
                    routeList.add(new SimpleImmutableEntry<IStreet, IStop>(str, stop));
                    atLeastOne = true;
                }
            }
            if (!atLeastOne){
                routeList.add(new SimpleImmutableEntry<IStreet, IStop>(str, null));
            }
        }

        return routeList;
    }
}

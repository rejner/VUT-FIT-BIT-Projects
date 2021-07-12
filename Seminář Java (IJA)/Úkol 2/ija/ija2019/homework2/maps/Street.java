package ija.ija2019.homework2.maps;

import ija.ija2019.homework2.interfaces.*;

import java.util.ArrayList;
import java.util.List;

public class Street implements IStreet {

    private String id;
    private List<Coordinate> coordinatesList;
    private List<IStop> stopsList;

    private Coordinate beginCoordinate, endCoordinate;

    public static Street defaultStreet(String id, Coordinate... coordinates){

        Street str = new Street();
        str.id = id;
        str.coordinatesList = new ArrayList<Coordinate>();
        str.stopsList = new ArrayList<IStop>();

        Coordinate lastCoordinate = null;
        for (Coordinate c : coordinates){
            // if no lastCoordinate is set, we are at the beginning of coordinates
            if (lastCoordinate == null){
                str.beginCoordinate = c;
            } else {
                // if both coordinates have changed, there can be no right rectangular shape
                if ((c.getX() != lastCoordinate.getX()) && (c.getY() != lastCoordinate.getY())){
                    str = null;
                    break;
                }
            }
            str.coordinatesList.add(c); // add coordinate to coordinatesList
            lastCoordinate = c;
        }

        if (str != null){
            str.endCoordinate = str.coordinatesList.get(str.coordinatesList.size() - 1);
        }

        return str;
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
    public List<IStop> getStops() {
        return this.stopsList;
    }

    @Override
    public boolean addStop(IStop stop) {

        Coordinate lastCoordinate = null;
        for (Coordinate c : coordinatesList){
                if (lastCoordinate == null){
                    lastCoordinate = c;
                    continue;
                }

                if (c.diffX(lastCoordinate) != 0) {
                    if (stop.getCoordinate().getY() == c.getY()) {

                        if (((lastCoordinate.getX() <= stop.getCoordinate().getX()) && (stop.getCoordinate().getX() <= c.getX()))
                                || ((lastCoordinate.getX() >= stop.getCoordinate().getX()) && (stop.getCoordinate().getX() >= c.getX()))) {
                            stopsList.add(stop);
                            stop.setStreet(this);
                            return true;
                        }
                    }
                } else if (c.diffY(lastCoordinate) != 0){
                    if (stop.getCoordinate().getX() == c.getX()){

                        if (((lastCoordinate.getY() <= stop.getCoordinate().getY()) && (stop.getCoordinate().getY() <= c.getY()))
                            || ((lastCoordinate.getY() >= stop.getCoordinate().getY()) && (stop.getCoordinate().getY() >= c.getY()))){
                            stopsList.add(stop);
                            stop.setStreet(this);
                            return true;
                        }
                    }
                }
                lastCoordinate = c;
        }
        return false;
    }

    @Override
    public Coordinate begin(){
        return beginCoordinate;
    }

    @Override
    public Coordinate end(){
        return endCoordinate;
    }

    @Override
    public boolean follows(IStreet s){
        if ((s.end().equals(endCoordinate) || s.end().equals(beginCoordinate)) ||
                (s.begin().equals(beginCoordinate) || s.begin().equals(endCoordinate))){
            return true;
        }
        return false;
    }
}

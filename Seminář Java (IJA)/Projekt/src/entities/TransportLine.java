package entities;

import java.util.ArrayList;
import java.util.List;
import java.util.AbstractMap.SimpleImmutableEntry;

/**
 * TransportLine class represents one unique transport line. It holds information about it's id, stops and streets.
 * Instance of this class can be passed to multiple vehicles, which then share the same route, line number but varies
 * in their unique vehicle number id and position based on time of their creation.
 * @author Michal Rein, Ondřej Motyčka
 */
public class TransportLine {

    /** Id number of transport line.*/
    private String id;
    /** Lists with stops, streets and vehicles (not objects, just information about their number and time of deployment).*/
    private List<Stop> stopList;
    private List<Street> streetList;
    private List<SimpleImmutableEntry<String, String>> vehicleList;

    /** Constructor that creates new transport line containing only it's id. It's lists are being filled externally.
     * @param id Line number.
     * @return New TransportLine object.
     * */
    public static TransportLine createLine(String id){
        TransportLine transportLine = new TransportLine();
        transportLine.id = id;
        transportLine.stopList = new ArrayList<Stop>();
        transportLine.streetList = new ArrayList<Street>();
        transportLine.vehicleList = new ArrayList<SimpleImmutableEntry<String, String>>();
        return transportLine;
    }

    /** Add stop object into list of stops.
     * @param stop Stop object which should be added.
     * @return Status of successful addition of a stop to the stop list.
     * */
    public boolean addStop(Stop stop){

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

    /** Add street into list of streets.
     * @param street Street object which should be added.
     * @return Status of successful addition of a street to the street list.
     * */
    public boolean addStreet(Street street){

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

    /**
     * Add information about vehicle into vehicle list.
     * @param entry Information about vehicle (number + deploy time)
     */
    public void addVehicle(SimpleImmutableEntry<String, String> entry){
        vehicleList.add(entry);
    }

    /** Takes streets and stop from lists and create a new list of SimpleImmutableEntries representing route.
     * @return List representing transport line's route.
     * */
    public List<SimpleImmutableEntry<Street, Stop>> createRoute(){

        List<SimpleImmutableEntry<Street, Stop>> routeList = new ArrayList<>();

        for (Street str : streetList){

            Boolean atLeastOne = false;
            for (Stop stop : stopList){
                if (str.getId() == stop.getStreet().getId()){
                    routeList.add(new SimpleImmutableEntry<Street, Stop>(str, stop));
                    atLeastOne = true;
                }
            }
            if (!atLeastOne){
                routeList.add(new SimpleImmutableEntry<Street, Stop>(str, null));
            }
        }

        return routeList;
    }

    /**
     * Get identification number of transport line.
     * @return Transport line's number.
     */
    public String getId() {
        return id;
    }

    public List<SimpleImmutableEntry<String, String>> getVehicleList(){
        return vehicleList;
    }
}

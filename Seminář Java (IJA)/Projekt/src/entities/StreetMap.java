package entities;

import controls.AlertBox;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import org.json.*;
import org.json.JSONObject;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.List;

/**
 * Class StreetMap is a representation of actual map. It holds information about all stops, streets and transport lines.
 * New Pane is created where all streets, stops and vehicles will be displayed.
 * Layout of streets and stops is loaded from JSON file in specific format.
 * @author Michal Rein, Ondřej Motyčka
 */
public class StreetMap {

    /** Pane for displaying map and other entities.*/
    private Pane mapCanvas;
    /** Lists with streets, stops and transport lines.*/
    private List<Street> streetList;
    private List<Stop> stopList;
    private List<TransportLine> transportLineList;
    /** JSON file representing map and transport lines.*/
    private File mapFile;

    /**
     * Initialize lists for storing streets, stops and lines. Parse given JSON file and draw map.
     * @param canvas Pane canvas.
     * @param file JSON file with map representation.
     */
    public StreetMap(Pane canvas, File file){
        this.mapFile = file;
        this.mapCanvas = canvas;
        this.streetList = new ArrayList<Street>();
        this.stopList = new ArrayList<Stop>();
        this.transportLineList = new ArrayList<TransportLine>();
        parseJson();
        drawMap();
    }

    /**
     * Draw all extracted streets and stops onto pane canvas.
     */
    public void drawMap(){

        for (Street street : streetList){
            Line line = new Line(street.begin().getX(), street.begin().getY(), street.end().getX(), street.end().getY());
            line.setStroke(Color.BLACK);
            line.setStrokeWidth(2);
            street.setStrLine(line);

            for (Stop stop : street.getStops()){
                Rectangle stopMark = new Rectangle(stop.getCoordinate().getX() - 3, stop.getCoordinate().getY() - 3, 6, 6);
                stopMark.setFill(Color.RED);
                stop.setStopMark(stopMark);
                mapCanvas.getChildren().add(stopMark);
            }
            street.setStreetTextObjLayout();
            mapCanvas.getChildren().addAll(line, street.getStreetTextObj());

        }
    }

    /**
     * Open JSON file and parse it's object into separate Java Class instances.
     */
    private void parseJson(){
        try{
            // JSON to string
            //String jsonContent = new String(Files.readAllBytes(Paths.get("D://Programování//Java//IJA-Project//data//examples//example.json")));
            String jsonContent = new String(Files.readAllBytes(Paths.get(mapFile.getPath())));

            JSONObject obj = null;
            JSONArray streets = null, lines = null;


            // Create JSON object, extract streets and lines into an array
            try{
                obj = new JSONObject(jsonContent);
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Failed to read content of JSON file, format is invalid!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(2);
            }

            try{
                streets = (JSONArray) obj.get("streets");
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't parse array containing information about streets!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(2);
            }

            try{
                lines = (JSONArray) obj.get("lines");
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't parse array containing information about lines!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(2);
            }

            // for each street object, parse it
            streets.forEach(street -> parseStreet((JSONObject) street));
            // for each line object, parse it
            lines.forEach(line -> parseLine((JSONObject) line));

        } catch (IOException exception){
            System.out.println(exception);
            System.exit(1);
        }
    }

    /**
     * Parse JSON street and extract all stops from "stops" array.
     * @param street Street in JSONObject format.
     */
    private void parseStreet(JSONObject street){

        String street_id = null;
        JSONArray begin = null, end = null;
        Integer begin_x = null, begin_y = null, end_x = null, end_y = null, congestion_level = null;

        // extract street's name
        try{
            street_id = (String) street.get("streetName");
        } catch (Exception ex){
            AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                                "Couldn't extract 'streetName' attribute!\n\n" +
                                "Error in file: " + mapFile.getPath());
            System.exit(3);
        }

        // extract street's begin coordinate
        try{
            begin = (JSONArray) street.get("begin");
            begin_x = begin.getInt(0);
            begin_y = begin.getInt(1);

        } catch (Exception ex){
            AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                    "Couldn't extract begin coordinates of street object "+ street_id + "! \n\n" +
                    "Error in file: " + mapFile.getPath());
            System.exit(3);
        }

        // extract street's end coordinate
        try{
            end = (JSONArray) street.get("end");
            end_x = end.getInt(0);
            end_y = end.getInt(1);
        } catch (Exception ex){
            AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                    "Couldn't extract end coordinates of street object "+ street_id + "! \n\n" +
                    "Error in file: " + mapFile.getPath());
            System.exit(3);
        }

        // extract street's congestion level
        try{
            congestion_level = (Integer) street.get("congestion");
            if (congestion_level < 0 || congestion_level > 3){
                throw new Exception("Incorrect level of congestion");
            }
        } catch (Exception ex){
            AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                    "Couldn't extract congestion level of street object "+ street_id + "!\n" +
                    "Attribute might be missing or incorrect value of congestion level (only 0, 1, 2, 3 supported).\n\n" +
                    "Error in file: " + mapFile.getPath());
            System.exit(3);
        }


        Coordinate begin_coordinate = Coordinate.create(begin_x, begin_y);
        Coordinate end_coordinate = Coordinate.create(end_x, end_y);

        Street str = Street.createStreet(street_id, congestion_level, begin_coordinate, end_coordinate);

        // test if street was created successfully
        if (str == null){
            AlertBox.display("Street creation error", "Street " + street_id + " couldn't be created.\n" +
                    "This might be caused by creation of non-straight street (remember that this app only supports\n" +
                    "straight horizontal or vertical streets). Please, check street's coordinates and fix this error...\n\n" +
                    "Error in file: " + mapFile.getPath());
            System.exit(3);
        }


        JSONArray stops = null;

        // extract list of stops that are located on current street
        try{
            stops = (JSONArray) street.get("stops");
        } catch (Exception ex){
            AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                    "Couldn't extract stops from street object "+ street_id + "!\n\n" +
                    "Error in file: " + mapFile.getPath());
            System.exit(3);
        }

        // iterate through list of stops
        stops.forEach(stop -> {

            String stop_id = null;
            JSONObject stopObj = null;
            JSONArray stop_coordinates = null;
            Integer stop_x = null, stop_y = null;

            // extract stop object
            try{
                stopObj = (JSONObject) stop;
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't parse stop object!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(3);
            }

            // extract stop's name
            try{
                stop_id = (String) stopObj.get("stopName");
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't get 'stopName' attribute from a stop object!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(3);
            }

            // extract stop's coordinates
            try{
                stop_coordinates = (JSONArray) stopObj.get("coordinates");
                stop_x = (Integer) stop_coordinates.getInt(0);
                stop_y = (Integer) stop_coordinates.getInt(1);
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't extract coordinates of stop " + stop_id +"!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(3);
            }

            // create new Stop class
            Stop stopClass = new Stop(stop_id, Coordinate.create(stop_x, stop_y));

            // try to add stop to the street, test if stop was successfully added
            if (!str.addStop(stopClass)){
                AlertBox.display("Stop creation error", "Stop " + stopClass.getId() + " couldn't be added to the street " + str.getId() +"!\n" +
                                "Stop's coordinates don't match position of the street, please fix this error...\n\n" +
                                "Error in file: " + mapFile.getPath());
                System.exit(3);
            }
            stopList.add(stopClass);
        });
        streetList.add(str);
    }

    /**
     * Parse transport line from JSONObject. For each line create new TransportLine class instance.
     * @param line Transport line in JSONObject format.
     */
    private void parseLine(JSONObject line){

        String lineNumber = null;
        JSONArray route = null, vehicles = null;

        // extract line's number
        try{
            lineNumber = (String) line.get("lineNumber");
        } catch (Exception ex){
            AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                    "Couldn't extract 'lineNumber' attribute from line object!\n\n" +
                    "Error in file: " + mapFile.getPath());
            System.exit(4);
        }

        // extract line's route
        try{
            route = (JSONArray) line.get("route");
        } catch (Exception ex){
            AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                    "Couldn't extract route of the line " + lineNumber +"!\n\n" +
                    "Error in file: " + mapFile.getPath());
            System.exit(4);
        }

        // extract line's vehicles
        try{
            vehicles = (JSONArray) line.get("vehicles");
        } catch (Exception ex){
            AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                    "Couldn't extract route of the line " + lineNumber +"!\n\n" +
                    "Error in file: " + mapFile.getPath());
            System.exit(4);
        }

        TransportLine transportLine = TransportLine.createLine(lineNumber);

        // parse route
        route.forEach(path -> {
            JSONObject pathObj = null;
            String street_id = null;
            JSONArray stops = null;

            // extract path
            try{
                pathObj = (JSONObject) path;
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't extract full route from the route object!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(4);
            }

            // extract street's name of a path
            try{
                street_id = (String) pathObj.get("streetName");
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't extract 'streetName' attribute from the route object!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(4);
            }

            // extract stops on current street
            try{
                stops = (JSONArray) pathObj.get("stops");
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't extract stops from the route object!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(4);
            }


            // if array with stops id's isn't empty, search for matching stop object in stopList and add it to the transport line object
            if (!stops.isEmpty()){

                // for each stop in JSON "stops" property
                for (Object stop : stops){

                    String stop_id = null;

                    // extract stop's name
                    try{
                        stop_id = (String) stop;
                    } catch (Exception ex){
                        AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                                "Couldn't extract stop!\n\n" +
                                "Error in file: " + mapFile.getPath());
                        System.exit(4);
                    }


                    // search for it's matching class in stopList and add it to the transport line
                    for (Stop stopClass : stopList){
                        if (stopClass.getId().equals(stop_id)){
                            transportLine.addStop(stopClass);
                            break;
                        }
                    }
                }
            } else {
                // add street to the transport line if line only pass by street without any stops
                for (Street streetClass : streetList){
                    if (streetClass.getId().equals(street_id)){

                        // create auxiliary stop in the middle of a street
                        // this stop will occur only in line's route and will not affect stopLists
                        int auxX = (streetClass.begin().getX() + streetClass.end().getX())/2;
                        int auxY = (streetClass.begin().getY() + streetClass.end().getY())/2;
                        Coordinate tmpCoord = Coordinate.create(auxX, auxY);
                        Stop auxStop = new Stop("auxStop", tmpCoord);
                        auxStop.setStreet(streetClass);
                        auxStop.setIsAuxStop();

                        transportLine.addStop(auxStop);
                        //transportLine.addStreet(streetClass);
                        break;
                    }
                }
            }
        });

        // parse vehicles
        vehicles.forEach(vehicle -> {
            JSONObject vehicleObj = null;
            String vehicle_id = null, vehicle_deploy = null;

            // extract vehicle JSON object
            try{
                vehicleObj = (JSONObject) vehicle;
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't extract vehicle!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(4);
            }

            // extract vehicle's id
            try{
                vehicle_id = (String) Integer.toString((int) vehicleObj.get("id"));
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't extract vehicle's id!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(4);
            }

            // extract time of deployment
            try{
                vehicle_deploy = (String) vehicleObj.get("deploy");
            } catch (Exception ex){
                AlertBox.display("JSON parse error", "An error occurred during JSON parse.\n" +
                        "Couldn't extract 'deploy' attribute from vehicle " + vehicle_id + "!\n\n" +
                        "Error in file: " + mapFile.getPath());
                System.exit(4);
            }

            transportLine.addVehicle(new AbstractMap.SimpleImmutableEntry<String, String>(vehicle_id, vehicle_deploy));
        });
        // add transport line to the global list of transport lines
        transportLineList.add(transportLine);
    }

    /**
     * Get list of transport lines present on the map.
     * @return List of transport lines.
     */
    public List<TransportLine> getTransportLineList(){
        return transportLineList;
    }

    /**
     * Get list of all streets present on the map.
     * @return List of all streets.
     */
    public List<Street> getStreetList(){ return streetList; }

}

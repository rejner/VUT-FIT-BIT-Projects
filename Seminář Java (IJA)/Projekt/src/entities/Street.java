package entities;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.scene.control.Label;
import javafx.scene.effect.DropShadow;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.text.TextAlignment;
import java.util.ArrayList;
import java.util.List;


/**
 * Street class represents one unique street in the map simulated environment. It holds information about its id, level of
 * congestion and stops that are present on the street.
 * @author Michal Rein, Ondřej Motyčka
 */
public class Street {

    /** Unique identifier. */
    private String id;
    /** List of coordinates that describe the street. */
    private List<Coordinate> coordinatesList;
    /** List of stops that are present on the street. */
    private List<Stop> stopsList;
    /** JavaFX Line object. */
    private Line strLine;
    /** Level of congestion property. */
    private IntegerProperty congestionLevel = new SimpleIntegerProperty(1);
    /** Beginning and end coordinates. */
    Coordinate beginCoordinate, endCoordinate;
    /** Label displaying street's name. */
    private Label streetTextObj;
    /** Indicator whether congestion level should be shown or not. */
    private boolean congestionShowed = false;

    /**
     * New Street creator. Check for coordinates validity is performed, Street object is returned on success.
     * @param id Name of the street.
     * @param cLevel Congestion level.
     * @param coordinates Coordinates that describe street.
     * @return New valid Street object.
     */
    public static Street createStreet(String id, int cLevel, Coordinate... coordinates){
        Street str = new Street();
        str.id = id;
        str.coordinatesList = new ArrayList<Coordinate>();
        str.stopsList = new ArrayList<Stop>();

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

            str.streetTextObj = new Label(id);
            str.streetTextObj.setTextAlignment(TextAlignment.CENTER);

            str.endCoordinate = str.coordinatesList.get(str.coordinatesList.size() - 1);
            str.setCongestionLevel(cLevel);
        }
        return str;
    }

    /**
     * Get street identifier.
     * @return Street identifier (name).
     */
    public String getId(){
        return this.id;
    }

    /**
     * Get List of street coordinates.
     * @return List of street coordinates.
     */
    public List<Coordinate> getCoordinates(){
        return this.coordinatesList;
    }

    /**
     * Get List of stops associated with street.
     * @return List of stops.
     */
    public List<Stop> getStops() {
        return this.stopsList;
    }

    /**
     * Get street name Label object.
     * @return Street name Label object.
     */
    public Label getStreetTextObj(){
        return streetTextObj;
    }

    // add stop to the street, perform validation of coordinates given

    /**
     * Add stop to the street's stop list, also check if position of a stop is valid.
     * @param stop Stop object that should be added to the street's stop list.
     * @return Result of operation.
     */
    public boolean addStop(Stop stop) {

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

    /**
     * Get begin coordinate of a street.
     * @return Begin Coordinate object.
     */
    public Coordinate begin(){
        return beginCoordinate;
    }

    /**
     * Get end coordinate of a street.
     * @return End Coordinate object.
     */
    public Coordinate end(){
        return endCoordinate;
    }

    /**
     * Set Line object displaying street on the map.
     * @param l Line object of a street.
     */
    public void setStrLine(Line l){
        strLine = l;
    }

    /**
     * Set level of congestion.
     * @param value Required level of congestion.
     */
    public void setCongestionLevel(int value){
        congestionLevel.setValue(value);
    }

    /**
     * Set position of text (Label) object that displays street name.
     */
    public void setStreetTextObjLayout(){

        double x = beginCoordinate.getX(), y = beginCoordinate.getY();
        int idLength = id.length();

        // street is horizontal
        if (beginCoordinate.diffX(endCoordinate) != 0){
            x = ((beginCoordinate.getX() + endCoordinate.getX())/2.0) - idLength*2.0;
            y = beginCoordinate.getY() - 20;
        }

        // street is vertical
        if (beginCoordinate.diffY(endCoordinate) != 0){
            streetTextObj.setRotate(90);
            y = ((beginCoordinate.getY() + endCoordinate.getY())/2.0);
            if (idLength >= 10){
                x = beginCoordinate.getX() - idLength*2;
            } else {
                x = beginCoordinate.getX();
            }
        }

        if (x < 0) x *= -1;
        if (y < 0) y *= -1;

        streetTextObj.setLayoutX(x);
        streetTextObj.setLayoutY(y);

        streetTextObj.setVisible(false);
        //streetTextObj.setStyle("-fx-font-weight: bold");
    }

    /**
     * Highlight street.
     */
    public void highlightLine(){

        // change color and width
        strLine.setStroke(Color.BLUE);
        strLine.setStrokeWidth(4);
        streetTextObj.setTextFill(Color.BLUE);

        // glow effect
        DropShadow borderGlow= new DropShadow();
        borderGlow.setOffsetY(0f);
        borderGlow.setOffsetX(0f);
        borderGlow.setRadius(5);
        borderGlow.setColor(Color.BLUE);
        strLine.setEffect(borderGlow);
    }

    /**
     * Unhighlight street.
     */
    public void unhighlightLine(){
        strLine.setStroke(Color.BLACK);
        strLine.setStrokeWidth(2);
        streetTextObj.setTextFill(Color.BLACK);
        strLine.setEffect(null);
        if (congestionShowed) showCongestionLevel();
    }

    /**
     * Change street line color based on level of congestion.
     */
    public void showCongestionLevel(){

        congestionShowed = true;
        if (strLine.getStroke().equals(Color.BLUE)) return;
        switch (congestionLevel.getValue()){
            case 0:
                strLine.setStroke(Color.GREEN);
                break;
            case 1:
                strLine.setStroke(Color.YELLOWGREEN);
                break;
            case 2:
                strLine.setStroke(Color.ORANGE);
                break;
            case 3:
                strLine.setStroke(Color.RED);
                break;
            default:
                strLine.setStroke(Color.BLACK);
                break;
        }
    }

    /**
     * Show street names on the map.
     */
    public void showStreetNames(){
        streetTextObj.setVisible(true);
    }

    /**
     * Hide street names on the map.
     */
    public void hideStreetNames(){
        streetTextObj.setVisible(false);
    }

    /**
     * Hide congestion level, change color to black.
     */
    public void hideCongestionLevel(){
        congestionShowed = false;
        if (strLine.getStroke().equals(Color.BLUE)) return;
        strLine.setStroke(Color.BLACK);
    }

    /**
     * Get street Line object.
     * @return Street Line object.
     */
    public Line getStrLine(){
        return strLine;
    }

    /**
     * Get level of congestion.
     * @return Level of congestion.
     */
    public int getCongestionLevel(){
        return congestionLevel.getValue();
    }

    /**
     * Check if given street follows this street.
     * @param s Street object to check follow with.
     * @return True/False, depending on whether streets follow each other or not.
     */
    public boolean follows(Street s){
        if ((s.end().equals(endCoordinate) || s.end().equals(beginCoordinate)) ||
                (s.begin().equals(beginCoordinate) || s.begin().equals(endCoordinate))){
            return true;
        }
        return false;
    }
}

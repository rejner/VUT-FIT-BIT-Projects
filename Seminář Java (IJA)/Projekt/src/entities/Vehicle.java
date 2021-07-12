package entities;

import javafx.scene.effect.DropShadow;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.text.Text;
import javafx.scene.text.TextAlignment;
import controls.StatusBarLine;
import controls.Time;
import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.List;

/**
 * Class Vehicle represents specific vehicle shown on the map.
 * @author Michal Rein, Ondřej Motyčka
 */
public class Vehicle extends Circle {

    /** Specific transport line, multiple vehicle can refer to the same line.*/
    private TransportLine line;
    /** Handle for Line's status bar.*/
    private StatusBarLine statusBarLine;
    /** Line number.*/
    private String lineNumber;
    /** Unique number of vehicle.*/
    private String vehicleNumber;
    /** DX and DY determine directions on X and Y axes.*/
    private double dx;
    private double dy;
    /** Essential coordinates*/
    private Stop lastStop, nextStop, startStop, endStop;
    /** Special indicators to determine where is vehicle currently located*/
    private int currentStopIndex, timeTableIndex;

    /** List of stops, full route and list of times of arrival.*/
    private List<Stop> stopList;
    private List<AbstractMap.SimpleImmutableEntry<Street, Stop>> route;
    private List<AbstractMap.SimpleImmutableEntry<String, Integer>> timetable;

    /** Indicates whether vehicle has already reached it's final destination and is on it's way back.*/
    private boolean reverse = false;
    /** Indicates whether vehicle is being displayed on status bar.*/
    private boolean displayed = false;
    /** Rendered text of line number which is associated with vehicle*/
    private Text lineTextObj;
    /** Timeout for simulating 60 seconds of waiting on stops.*/
    int timeout;
    long deployTime;
    boolean deployed = false;

    /**
     * Create new vehicle, get route, set mark properties, essential coordinates and calculate direction towards first stop.
     * @param transportLine TransportLine class that the vehicle is part of.
     * @param color Color of vehicle shown on the map.
     * @param number Unique number of vehicle.
     * @param deploy Time (is seconds format) when vehicle should be deployed.
     */
    public Vehicle(TransportLine transportLine, Color color, String number, Long deploy){

        line = transportLine;
        lineNumber = line.getId();
        vehicleNumber = number;
        currentStopIndex = 1;
        timeTableIndex = 1;
        timeout = 0;
        deployTime = deploy;

        route = new ArrayList<>();
        route = line.createRoute();

        timetable = new ArrayList<>();

        lineTextObj = new Text(lineNumber);
        lineTextObj.setTextAlignment(TextAlignment.CENTER);
        lineTextObj.setStyle("-fx-font-weight: bold");
        setLineTextObjLayout(route.get(0).getValue().getCoordinate().getX(), route.get(0).getValue().getCoordinate().getY());

        this.setRadius(12);
        this.setFill(color);
        this.setLayoutX(route.get(0).getValue().getCoordinate().getX());
        this.setLayoutY(route.get(0).getValue().getCoordinate().getY());

        lastStop = route.get(0).getValue();

        // if next path in route has no stop, next stop must be found
        if (route.get(currentStopIndex).getValue() == null){
            route.forEach(path -> {
                    // if this street has a stop
                    if (path.getValue() != null){
                        // if it's not a starting stop
                        if (! path.getValue().getId().equals(route.get(0).getValue().getId())){
                            // assign coordinates of the next stop to the nextStop variable
                            nextStop = path.getValue();
                        }
                    }
            });
        } else {
            nextStop = route.get(currentStopIndex).getValue();
        }

        startStop = route.get(0).getValue();
        endStop = route.get(route.size() - 1).getValue();

        // hide vehicle for now
        lineTextObj.setVisible(false);
        setVisible(false);

        calculateDirection();

    }

    /**
     * Calculates trajectory of vehicle and sets it's new layout coordinates
     */
    public void calculateTrajectory(){

        int x = (int) getLayoutX();
        int y = (int) getLayoutY();

        // check whether vehicle reached stop coordinates or not
        if (x == getNextStop().getCoordinate().getX()) {
            if (y == getNextStop().getCoordinate().getY()) {
                waitOnStop();
            } else {

                // check end and start of the street
                if (x == getRoute().get(getCurrentStopIndex()).getKey().begin().getX() &&
                        y == getRoute().get(getCurrentStopIndex()).getKey().begin().getY()){
                    calculateDirection();
                } else if (x == getRoute().get(getCurrentStopIndex()).getKey().end().getX() &&
                        y == getRoute().get(getCurrentStopIndex()).getKey().end().getY()){
                    calculateDirection();
                }

                // set new X and Y coordinates (on Pane plain)
               setLayoutX(getLayoutX() + getDx());
               setLayoutY(getLayoutY() + getDy());
               setLineTextObjLayout(getLayoutX(), getLayoutY());
            }

            // check if vehicle is at the beginning of current street
        } else if (x == getRoute().get(getCurrentStopIndex()).getKey().begin().getX() &&
                y == getRoute().get(getCurrentStopIndex()).getKey().begin().getY()) {

            // calculate velocity
            calculateDirection();
            // set new X and Y coordinates (on Pane plain)
            setLayoutX(getLayoutX() + getDx());
            setLayoutY(getLayoutY() + getDy());
            setLineTextObjLayout(getLayoutX(), getLayoutY());

            // check if vehicle is at the end of current street
        } else if (x == getRoute().get(getCurrentStopIndex()).getKey().end().getX() &&
                y == getRoute().get(getCurrentStopIndex()).getKey().end().getY()) {

            // calculate velocity
            calculateDirection();
            // set new X and Y coordinates (on Pane plain)
            setLayoutX(getLayoutX() + getDx());
            setLayoutY(getLayoutY() + getDy());
            setLineTextObjLayout(getLayoutX(), getLayoutY());

        } else {
            // set new X and Y coordinates (on Pane plain)
            setLayoutX(getLayoutX() + getDx());
            setLayoutY(getLayoutY() + getDy());
            setLineTextObjLayout(getLayoutX(), getLayoutY());
        }

        if (displayed){
            statusBarLine.setPosition(getLayoutX(), getLayoutY());

            // show next and last stops on the status bar
            if (!reverse){

                if (!lastStop.getIsAuxStop()){
                    statusBarLine.setLastStopProperty(lastStop.getId());
                }

                if (!nextStop.getIsAuxStop()){
                    statusBarLine.setNextStopProperty(nextStop.getId());
                } else {
                    int i = 0;
                    while (route.get(currentStopIndex + i).getValue().getIsAuxStop()){
                        i++;
                    }
                    statusBarLine.setNextStopProperty(route.get(currentStopIndex + i).getValue().getId());
                }

            } else {

                if (!lastStop.getIsAuxStop()){
                    statusBarLine.setLastStopProperty(lastStop.getId());
                }

                if (!nextStop.getIsAuxStop()){
                    statusBarLine.setNextStopProperty(nextStop.getId());
                } else {
                    int i = 0;
                    while (route.get(currentStopIndex - i).getValue().getIsAuxStop()){
                        i++;
                    }
                    statusBarLine.setNextStopProperty(route.get(currentStopIndex - i).getValue().getId());
                }
            }

            statusBarLine.setTimeUntilNSProperty(statusBarLine.getTimeHandle().calculateTimeDifference(timetable.get(timeTableIndex).getValue()));
        }
    }

    /**
     * Called when vehicle reached a stop. Auxiliary stops are passed, regular stops invoke counter, which simulates
     * 60 seconds period of waiting at a stop.
     */
    public void waitOnStop(){

        // timer if vehicle is stopped
        if (timeout != 0){
            timeout++;
            if (timeout == 60){
                nextStop = route.get(currentStopIndex).getValue();
                calculateDirection();
                timeout = 0;
            }
            return;
        }

        // if current stop is the end, negate velocity
        if (nextStop.equals(endStop)){
            reverse = !reverse;

            // switch start with end
            Stop tmp = startStop;
            startStop = endStop;
            endStop = tmp;

            // reset time table index and create new time table
            timeTableIndex = 1;
            createTimetable();
            System.out.println("Vehicle " + lineNumber + " reached end of route...");

        // vehicle stopped at regular Stop on its way to final Stop
        } else {
            // calculateVelocity();
            if (!route.get(currentStopIndex).getValue().getIsAuxStop()){
                timeTableIndex++;
                System.out.println("Vehicle " + lineNumber + " waiting on a stop...");
            }
        }

        // if stop is regular stop, set timeout (doesn't affect auxiliary stops)
        if (!route.get(currentStopIndex).getValue().getIsAuxStop()){
            if (!reverse){
                currentStopIndex++;
            } else {
                currentStopIndex--;
            }
            timeout = 1;
            lastStop = nextStop;
        // when auxiliary stop has been reached, set nexStop and calculate direction right away (without timer)
        } else {
            if (!reverse){
                currentStopIndex++;
            } else {
                currentStopIndex--;
            }
            nextStop = route.get(currentStopIndex).getValue();
            calculateDirection();
        }

        //nextStop = route.get(currentStopIndex).getValue().getCoordinate();


    }

    /**
     * Calculates direction to a next stop. If stops are on two different streets, determine which end of current street
     * neighbours with street of the next stop and set direction towards that neighbouring end. There should always be
     * a regular or an auxiliary (invisible) stop on any of neighbouring streets.
     */
    public void calculateDirection() {

        // next stop is on different street (both coordinates differ)
        if (nextStop.getCoordinate().getX() - (int) getLayoutX() != 0 && nextStop.getCoordinate().getY() - (int) getLayoutY() != 0){

            System.out.println("Calculation of direction for vehicle " + lineNumber + ", next stop is on different street.");

            // beginning of next street is on the same X coordinate
            if (route.get(currentStopIndex).getKey().begin().getX() - (int) getLayoutX() == 0){
                if (route.get(currentStopIndex).getKey().begin().getY() - (int) getLayoutY() < 0){
                    setDy(-1);
                } else {
                    setDy(1);
                }
                setDx(0);

            // end of next street is on the same X coordinate
            } else if (route.get(currentStopIndex).getKey().end().getX() - (int) getLayoutX() == 0) {
                if (route.get(currentStopIndex).getKey().end().getY() - (int) getLayoutY() < 0) {
                    setDy(-1);
                } else {
                    setDy(1);
                }
                setDx(0);

            // beginning of next street is on the same Y coordinate
            } else if (route.get(currentStopIndex).getKey().begin().getY() - (int) getLayoutY() == 0){
                if (route.get(currentStopIndex).getKey().begin().getX() - (int) getLayoutX() < 0){
                    setDx(-1);
                } else {
                    setDx(1);
                }
                setDy(0);

            // end of next street is on the same Y coordinate
            } else if (route.get(currentStopIndex).getKey().end().getY() - (int) getLayoutY() == 0){
                if (route.get(currentStopIndex).getKey().end().getX() - (int) getLayoutX() < 0){
                    setDx(-1);
                } else {
                    setDx(1);
                }
                setDy(0);

            }

        // difference in X coordinates, DX must be calculated
        } else if (nextStop.getCoordinate().getX() - (int) getLayoutX() != 0) {
            System.out.println("Calculation of DX for vehicle " + lineNumber);
            if (nextStop.getCoordinate().getX() - (int) getLayoutX() <= 0) {
                setDx(-1);
            } else {
                setDx(1);
            }
            setDy(0);

        // difference in Y coordinates, DY must be calculated
        } else if (nextStop.getCoordinate().getY() - (int) getLayoutY() != 0) {
            System.out.println("Calculation of DY for vehicle " + lineNumber);
            if (nextStop.getCoordinate().getY() - getLayoutY() >= 0) {
                setDy(1);
            } else {
                setDy(-1);
            }
            setDx(0);
        }
    }

    /**
     * Creates timetable by calculating estimated times of arrival between stops. A new timetable is created when vehicle
     * is created and each time it reaches any end of the route (start or end stops).
     */
    public void createTimetable(){

        // clear time table
        timetable.clear();

        Stop startPoint = null;
        Stop nextPoint = null;
        int lastEstTime = (int) statusBarLine.getTimeHandle().getAppTime();
        int estTime = 0;
        // from start -> end
        if (!reverse){

            startPoint = route.get(0).getValue();
            for (AbstractMap.SimpleImmutableEntry<Street, Stop> path : route){

                if (nextPoint != null && lastEstTime != 0 && !nextPoint.getIsAuxStop()){
                    estTime += 60; // add wait on stop delay (60 secs)
                }

                nextPoint = path.getValue();

                int tmpEstTime = 0;
                if (startPoint.getStreet().getId().equals(nextPoint.getStreet().getId())){ // stops are on the same street
                    tmpEstTime += startPoint.getCoordinate().diffX(nextPoint.getCoordinate()) + startPoint.getCoordinate().diffY(nextPoint.getCoordinate());
                    if (tmpEstTime < 0){
                        tmpEstTime *= -1;
                    }
                    estTime += tmpEstTime * Time.getCongestionConstant(startPoint.getStreet());
                } else { // stops are on neighboring streets

                    // if begin of current street neighbours with other street:
                    if (startPoint.getStreet().begin().equals(nextPoint.getStreet().begin()) || startPoint.getStreet().begin().equals(nextPoint.getStreet().end())){
                        int distToBeginOfCurStr = startPoint.getCoordinate().diffX(startPoint.getStreet().begin()) + startPoint.getCoordinate().diffY(startPoint.getStreet().begin());
                        int distFromBeginToNextStop = startPoint.getStreet().begin().diffX(nextPoint.getCoordinate()) + startPoint.getStreet().begin().diffY(nextPoint.getCoordinate());

                        if (distToBeginOfCurStr < 0) distToBeginOfCurStr *= -1;
                        if (distFromBeginToNextStop < 0) distFromBeginToNextStop *= -1;

                        distToBeginOfCurStr *= Time.getCongestionConstant(startPoint.getStreet());
                        distFromBeginToNextStop *= Time.getCongestionConstant(nextPoint.getStreet());

                        estTime += distToBeginOfCurStr + distFromBeginToNextStop;

                    // end of current street neighbours with other street:
                    } else {
                        int distToEndOfCurStr = startPoint.getCoordinate().diffX(startPoint.getStreet().end()) + startPoint.getCoordinate().diffY(startPoint.getStreet().end());
                        int distFromEndToNextStop = startPoint.getStreet().end().diffX(nextPoint.getCoordinate()) + startPoint.getStreet().end().diffY(nextPoint.getCoordinate());

                        if (distToEndOfCurStr < 0) distToEndOfCurStr *= -1;
                        if (distFromEndToNextStop < 0) distFromEndToNextStop *= -1;

                        distToEndOfCurStr *= Time.getCongestionConstant(startPoint.getStreet());
                        distFromEndToNextStop *= Time.getCongestionConstant(nextPoint.getStreet());

                        estTime += distToEndOfCurStr + distFromEndToNextStop;
                    }
                }

                // if nextPoint is and auxiliary stop, just save value and calculate further
                if (nextPoint.getIsAuxStop()){
                    startPoint = nextPoint;
                    continue;
                }

                estTime += lastEstTime; // add last estimated time
                timetable.add(new AbstractMap.SimpleImmutableEntry<String, Integer>(Time.convertTimeToString(estTime), estTime));
                startPoint = nextPoint;
                lastEstTime = estTime;
                estTime = 0;
            }

        // end -> start (vehicle is travelling in reverse order)
        } else {
            startPoint = route.get(route.size() - 1).getValue();

            for (int i = route.size() - 1; i >= 0; i--){
                if (nextPoint != null && lastEstTime != 0 && !nextPoint.getIsAuxStop()){
                    estTime += 60; // add wait on stop delay (60 secs)
                }

                nextPoint = route.get(i).getValue();

                int tmpEstTime = 0;
                if (startPoint.getStreet().getId().equals(nextPoint.getStreet().getId())){ // stops are on the same street
                    tmpEstTime += startPoint.getCoordinate().diffX(nextPoint.getCoordinate()) + startPoint.getCoordinate().diffY(nextPoint.getCoordinate());
                    if (tmpEstTime < 0){
                        tmpEstTime *= -1;
                    }
                    estTime += tmpEstTime * Time.getCongestionConstant(startPoint.getStreet());
                } else { // stops are on neighboring streets

                    // if begin of current street neighbours with other street:
                    if (startPoint.getStreet().begin().equals(nextPoint.getStreet().begin()) || startPoint.getStreet().begin().equals(nextPoint.getStreet().end())){
                        int distToBeginOfCurStr = startPoint.getCoordinate().diffX(startPoint.getStreet().begin()) + startPoint.getCoordinate().diffY(startPoint.getStreet().begin());
                        int distFromBeginToNextStop = startPoint.getStreet().begin().diffX(nextPoint.getCoordinate()) + startPoint.getStreet().begin().diffY(nextPoint.getCoordinate());

                        if (distToBeginOfCurStr < 0) distToBeginOfCurStr *= -1;
                        if (distFromBeginToNextStop < 0) distFromBeginToNextStop *= -1;

                        distToBeginOfCurStr *= Time.getCongestionConstant(startPoint.getStreet());
                        distFromBeginToNextStop *= Time.getCongestionConstant(nextPoint.getStreet());

                        estTime += distToBeginOfCurStr + distFromBeginToNextStop;

                    // end of current street neighbours with other street:
                    } else {
                        int distToEndOfCurStr = startPoint.getCoordinate().diffX(startPoint.getStreet().end()) + startPoint.getCoordinate().diffY(startPoint.getStreet().end());
                        int distFromEndToNextStop = startPoint.getStreet().end().diffX(nextPoint.getCoordinate()) + startPoint.getStreet().end().diffY(nextPoint.getCoordinate());

                        if (distToEndOfCurStr < 0) distToEndOfCurStr *= -1;
                        if (distFromEndToNextStop < 0) distFromEndToNextStop *= -1;

                        distToEndOfCurStr *= Time.getCongestionConstant(startPoint.getStreet());
                        distFromEndToNextStop *= Time.getCongestionConstant(nextPoint.getStreet());

                        estTime += distToEndOfCurStr + distFromEndToNextStop;
                    }
                }

                // if nextPoint is and auxiliary stop, just save value and calculate further
                if (nextPoint.getIsAuxStop()){
                    startPoint = nextPoint;
                    continue;
                }

                estTime += lastEstTime; // add last estimated time
                timetable.add(new AbstractMap.SimpleImmutableEntry<String, Integer>(Time.convertTimeToString(estTime), estTime));
                startPoint = nextPoint;
                lastEstTime = estTime;
                estTime = 0;
            }
        }

    }

    public List<AbstractMap.SimpleImmutableEntry<Street, Stop>>getRoute(){
        return route;
    }

    /**
     * Get index of the next stop to which vehicle is moving towards. By accessing route list with this index, next stop
     * will always be returned.
     * @return Index for accessing next stop in the route list.
     */
    public int getCurrentStopIndex(){
        return currentStopIndex;
    }

    /**
     * Get current DX of vehicle (direction and speed on X axis).
     * @return DX of vehicle.
     */
    public double getDx() {
        return dx;
    }

    /**
     * Get current DY of vehicle (direction and speed on Y axis).
     * @return DY of vehicle.
     */
    public double getDy(){
        return dy;
    }

    /**
     * Get coordinates of the next stop.
     * @return Coordinate object of the next stop.
     */
    public Stop getNextStop(){
        return nextStop;
    }

    /**
     * Get coordinates of the last passed stop.
     * @return Coordinate object of the last stop.
     */
    public Stop getLastStop(){
        return lastStop;
    }

    /**
     * Get line number of vehicle.
     * @return Line number.
     */
    public String getLineNumber(){
        return lineNumber;
    }

    /**
     * Get text object which is displayed on vehicle's mark.
     * @return Text object showing line number.
     */
    public Text getLineTextObj(){
        return lineTextObj;
    }

    /**
     * Get unique number of vehicle.
     * @return Vehicle's number.
     */
    public String getVehicleNumber(){
        return vehicleNumber;
    }

    /**
     * Get full unique signature in format: %lineNumber% (%vehicleNumber%).
     * @return Signature of vehicle.
     */
    public String getFullSignature(){
        return lineNumber + " (" + vehicleNumber + ")";
    }

    /**
     * Get vehicle's timetable.
     * @return List of times.
     */
    public List<AbstractMap.SimpleImmutableEntry<String, Integer>> getTimetable(){
        return timetable;
    }

    /**
     * Get boolean reverse value to determine whether vehicle is on it's way to final stop or on it's way back to start.
     * @return Boolean reverse value.
     */
    public boolean getReverse(){
        return reverse;
    }

    /**
     * Set DX of a vehicle.
     * @param value Value of DX.
     */
    public void setDx(double value){

        if (value == 0){
            dx = value;
            return;
        }

        boolean negative = false;
        double definitiveValue = value;
        if (value < 0) negative = true;
        int congestionLevel;

        // get congestion
        if (!reverse){

            // if stops are or different streets
            if (!route.get(currentStopIndex-1).getKey().equals(route.get(currentStopIndex).getKey())){

                // and vehicle is not on stop coordinates yet
                if (!(route.get(currentStopIndex-1).getValue().getCoordinate().getX() == (int) getLayoutX() &&
                    route.get(currentStopIndex-1).getValue().getCoordinate().getY() == (int) getLayoutY())){

                    // vehicle is on the border of streets, pick congestion level of the next stop's street
                    congestionLevel = route.get(currentStopIndex).getKey().getCongestionLevel();
                } else {
                    congestionLevel = route.get(currentStopIndex-1).getKey().getCongestionLevel();
                }

            // stops are on the same street, pick congestion of current street passing
            } else {
                congestionLevel = route.get(currentStopIndex-1).getKey().getCongestionLevel();
            }

        // vehicle is passing route reversal
        } else {
            // if routes aren't on the same street
            if (!route.get(currentStopIndex+1).getKey().equals(route.get(currentStopIndex).getKey())){
                
                if (!(route.get(currentStopIndex+1).getValue().getCoordinate().getX() == (int) getLayoutX() &&
                    route.get(currentStopIndex+1).getValue().getCoordinate().getY() == (int) getLayoutY())){
                    congestionLevel = route.get(currentStopIndex).getKey().getCongestionLevel();
                } else {
                    congestionLevel = route.get(currentStopIndex+1).getKey().getCongestionLevel();
                }
            } else {
                congestionLevel = route.get(currentStopIndex+1).getKey().getCongestionLevel();
            }
        }

        switch (congestionLevel){
            case 0:
                definitiveValue = 1;
                break;
            case 1:
                definitiveValue = 0.5;
                break;
            case 2:
                definitiveValue = 0.25;
                break;
            case 3:
                definitiveValue = 0.1;
                break;
        }

        if (negative) definitiveValue = definitiveValue * (-1);
        dx = definitiveValue;
    }

    /**
     * Set DY of a vehicle.
     * @param value Value of DY.
     */
    public void setDy(double value){

        if (value == 0){
            dy = value;
            return;
        }

        boolean negative = false;
        double definitiveValue = value;

        if (value < 0) negative = true;
        int congestionLevel;

        // get congestion
        if (!reverse){

            // if stops are or different streets
            if (!route.get(currentStopIndex-1).getKey().equals(route.get(currentStopIndex).getKey())){

                // and vehicle is not on stop coordinates yet
                if (!(route.get(currentStopIndex-1).getValue().getCoordinate().getX() == (int) getLayoutX() &&
                        route.get(currentStopIndex-1).getValue().getCoordinate().getY() == (int) getLayoutY())){

                    // vehicle is on the border of streets, pick congestion level of the next stop's street
                    congestionLevel = route.get(currentStopIndex).getKey().getCongestionLevel();
                } else {
                    congestionLevel = route.get(currentStopIndex-1).getKey().getCongestionLevel();
                }

                // stops are on the same street, pick congestion of current street passing
            } else {
                congestionLevel = route.get(currentStopIndex-1).getKey().getCongestionLevel();
            }

            // vehicle is passing route reversal
        } else {
            // if routes aren't on the same street
            if (!route.get(currentStopIndex+1).getKey().equals(route.get(currentStopIndex).getKey())){

                if (!(route.get(currentStopIndex+1).getValue().getCoordinate().getX() == (int) getLayoutX() &&
                        route.get(currentStopIndex+1).getValue().getCoordinate().getY() == (int) getLayoutY())){
                    congestionLevel = route.get(currentStopIndex).getKey().getCongestionLevel();
                } else {
                    congestionLevel = route.get(currentStopIndex+1).getKey().getCongestionLevel();
                }
            } else {
                congestionLevel = route.get(currentStopIndex+1).getKey().getCongestionLevel();
            }
        }

        switch (congestionLevel){
            case 0:
                definitiveValue = 1;
                break;
            case 1:
                definitiveValue = 0.5;
                break;
            case 2:
                definitiveValue = 0.25;
                break;
            case 3:
                definitiveValue = 0.1;
                break;
        }

        if (negative) definitiveValue = definitiveValue * (-1);
        dy = definitiveValue;
    }

    /**
     * Set line's status bar handle.
     * @param status Status bar handle.
     */
    public void setStatusBarLine(StatusBarLine status){
        statusBarLine = status;
    }

    public void setEvents(){
        // For each vehicle, set on click event + provide status bar pointer
        setOnMouseClicked(e -> statusBarLine.switchLine(statusBarLine.getActiveLine(), getLineNumber() + " (" + getVehicleNumber() + ")"));
        getLineTextObj().setOnMouseClicked(e -> statusBarLine.switchLine(statusBarLine.getActiveLine(), getLineNumber() + " (" + getVehicleNumber() + ")"));
        createTimetable();
    }

    /**
     * Set position of line's number text object.
     * @param x X value.
     * @param y Y value.
     */
    public void setLineTextObjLayout(double x, double y){

        if (Integer.parseInt(lineNumber) < 10){
            // 0...9
            lineTextObj.setLayoutX(x-3.5);
            lineTextObj.setLayoutY(y+4);
        } else if (Integer.parseInt(lineNumber) <= 99 ){
            lineTextObj.setLayoutX(x-7);
            lineTextObj.setLayoutY(y+4);
        } else {
            lineTextObj.setLayoutX(x-11);
            lineTextObj.setLayoutY(y+4);
        }
    }

    /**
     * Highlight a vehicle (set bigger radius, blue color and blue glow).
     * Also highlight all associated streets in route list.
     */
    public void highlightVehicle(){

        setRadius(20);
        DropShadow borderGlow= new DropShadow();
        borderGlow.setOffsetY(0f);
        borderGlow.setOffsetX(0f);
        borderGlow.setColor(Color.BLUE);
        setEffect(borderGlow);
        displayed = true;

        // highlight path
        route.forEach(path -> path.getKey().highlightLine());
    }

    /**
     * Annul highlight effect.
     */
    public void unhighlightVehicle(){
        setRadius(12);
        setEffect(null);
        displayed = false;

        // unhighlight path
        route.forEach(path -> path.getKey().unhighlightLine());
    }

    /**
     * Return deployment status of a vehicle.
     * @return Deployment status.
     */
    public boolean isDeployed(){
        return deployed;
    }

    /**
     * Compare simulated time with deployment time and determine if vehicle should appear
     * on the map.
     */
    public void checkDeployTime(){
        if (deployTime == statusBarLine.getTimeHandle().getAppTime()){
            deployed = true;
            setVisible(true);
            lineTextObj.setVisible(true);
            createTimetable();
        }
    }

}

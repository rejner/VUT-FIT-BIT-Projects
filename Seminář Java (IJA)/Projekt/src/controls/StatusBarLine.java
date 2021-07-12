package controls;

import entities.Stop;
import entities.Street;
import entities.Vehicle;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.geometry.HPos;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.Separator;
import javafx.scene.layout.GridPane;
import java.util.ArrayList;
import java.util.List;

/**
 * Class StatusBarLine represents part on the status bar (second part from the top) displaying information about
 * specific selected vehicle (current position, name, next stop, last stop...).
 * @author Michal Rein, Ondřej Motyčka
 */
public class StatusBarLine {

    /** Handle for Time control. */
    private Time timeHandle;
    /** Handle for status bar. */
    private GridPane statusBarHandle;
    /** Labels and their bound string properties. */
    private Label lineTitle, vehicleNumberTitle, vehicleNumber, linePositionTitle, linePosition,
                  lastStopTitle, lastStop, nextStopTitle, nextStop, timeUntilNSTitle, timeUntilNS;
    private StringProperty position, lastStopProperty, nextStopProperty, activeLine,
                            vehicleNumberString, timeUntilNSProperty;
    /** List of all vehicles present on the map. */
    private List<Vehicle> vehicles;
    /** Street and stop which are currently being displayed on the status bar. */
    private Street displayedStreet;
    private Stop displayedStop;
    /** Choice box for choosing vehicles. */
    private ChoiceBox chooseLineSelector;
    /** Buttons. */
    private Button showRouteBtn, editLineButton;

    /**
     *  Constructor of Line's status bar. Create and assign default values and call setStatus() method
     *  to create entire layout and set values.
     * @param statusBar Handle of entire status bar.
     * @param vehiclesList List of all vehicles.
     * */
    public StatusBarLine(GridPane statusBar, List vehiclesList){

        /*Create all required string properties and set default values*/
        activeLine = new SimpleStringProperty();
        position = new SimpleStringProperty();
        position.setValue("-");
        lastStopProperty = new SimpleStringProperty();
        lastStopProperty.setValue("-");
        nextStopProperty = new SimpleStringProperty();
        nextStopProperty.setValue("-");
        vehicleNumberString = new SimpleStringProperty();
        vehicleNumberString.setValue("-");
        timeUntilNSProperty = new SimpleStringProperty();
        timeUntilNSProperty.setValue("-");
        vehicles = new ArrayList<Vehicle>();
        vehicles = vehiclesList;

        statusBarHandle = statusBar;
        setStatus();
    }

    /**
     * Create layout, labels and buttons. All required values are extracted and set.
     */
    private void setStatus(){

        lineTitle = new Label("Line:");
            lineTitle.setPadding(new Insets(0, 0, 0, 10));
            GridPane.setConstraints(lineTitle, 0, 0);

        //Choice box for selecting various public transport lines
        ChoiceBox<String> chooseLine = new ChoiceBox<>();
            GridPane.setConstraints(chooseLine, 1, 0);
            this.chooseLineSelector = chooseLine;

        //getItems return the ObservableList object which you can add items to
        // for each vehicle in vehicle list, get its line number and add it to the choice box options
        vehicles.forEach((vehicle) -> chooseLine.getItems().add(vehicle.getLineNumber() + " (" + vehicle.getVehicleNumber() + ")"));
        chooseLine.setValue("0");
        chooseLine.setMaxWidth(80);
        activeLine.setValue("0");

        //Listen for selection changes -> changes values of properties
        chooseLine.getSelectionModel().selectedItemProperty().addListener( (v, oldLine, newLine) -> switchLine(oldLine, newLine));

        vehicleNumberTitle = new Label("Vehicle number:");
            vehicleNumberTitle.setPadding(new Insets(0, 0, 0, 10));
            GridPane.setConstraints(vehicleNumberTitle, 0, 1);

        vehicleNumber = new Label();
            vehicleNumber.textProperty().bind(vehicleNumberString);
            GridPane.setConstraints(vehicleNumber, 1, 1);

        linePositionTitle = new Label("Position:");
            linePositionTitle.setPadding(new Insets(0, 0, 0, 10));
            GridPane.setConstraints(linePositionTitle, 0, 3);

        linePosition = new Label();
            linePosition.textProperty().bind(position);
            GridPane.setConstraints(linePosition, 1, 3);

        lastStopTitle = new Label("Last Stop: ");
            lastStopTitle.setPadding(new Insets(0, 0, 0, 10));
            GridPane.setConstraints(lastStopTitle, 0, 4);

        lastStop = new Label();
            lastStop.textProperty().bind(lastStopProperty);
            GridPane.setConstraints(lastStop, 1, 4);

        nextStopTitle = new Label("Next Stop: ");
            nextStopTitle.setPadding(new Insets(0, 0, 0, 10));
            GridPane.setConstraints(nextStopTitle, 0, 5);

        nextStop = new Label();
            nextStop.textProperty().bind(nextStopProperty);
            nextStop.setPadding(new Insets(0, 0, 0, 0));
            GridPane.setConstraints(nextStop, 1, 5);

        timeUntilNSTitle = new Label("Next stop arrival: ");
            timeUntilNSTitle.setPadding(new Insets(0, 0, 10, 10));
            GridPane.setConstraints(timeUntilNSTitle, 0, 6);

        timeUntilNS = new Label();
            timeUntilNS.textProperty().bind(timeUntilNSProperty);
            timeUntilNS.setPadding(new Insets(0, 0, 10, 0));
            GridPane.setConstraints(timeUntilNS, 1, 6);

        showRouteBtn = new Button("Show route");
        GridPane.setColumnSpan(showRouteBtn, 2);
        GridPane.setConstraints(showRouteBtn, 0, 7);
        GridPane.setHalignment(showRouteBtn, HPos.CENTER);
        showRouteBtn.setAlignment(Pos.BASELINE_CENTER);
        showRouteBtn.maxWidth(200);
        showRouteBtn.setOnAction(e -> {
            String activeLine = getActiveLine();
            if (activeLine == null ||activeLine.equals("") || activeLine.equals("0")){
                AlertBox.display("Selection Error", "Please select a vehicle first in order to show it's route!");
                return;
            }
            ShowBox.displayRoute(activeLine, getActiveVehicle());
        });

        // Ready for edit module
        /*editLineButton = new Button("Edit");
        GridPane.setConstraints(editLineButton, 1, 7);
        GridPane.setHalignment(editLineButton, HPos.CENTER);
        editLineButton.setMaxSize(200, 100); */

        Separator statusSeparator = new Separator();
            statusSeparator.setPadding(new Insets(10, 0, 0, 0));
            GridPane.setConstraints(statusSeparator, 0, 8);
            GridPane.setColumnSpan(statusSeparator, 2);



        statusBarHandle.getChildren().addAll(lineTitle, chooseLine, vehicleNumberTitle, vehicleNumber, linePositionTitle, linePosition,
                                            lastStopTitle, lastStop, nextStopTitle, nextStop, showRouteBtn, statusSeparator,
                                            timeUntilNSTitle, timeUntilNS);

    }

    /**
     * This method is called whenever new vehicle is requested to be displayed on status bar.
     * Previously displayed vehicle must be set to it's default values (radius, highlight...) and the new one
     * is highlighted and displayed on status bar.
     * @param oldLine Old vehicle signature.
     * @param newLine New vehicle signature.
     */
    public void switchLine(String oldLine, String newLine){

        // System.out.println(oldLine + " ---> " + newLine); // FOR DEBUGGING PURPOSES

        // set new element on choice box (this is useful when user unselects vehicle and nothing should be displayed,
        // null pointer is passed and selected element in choice box is set to empty)
        chooseLineSelector.setValue(newLine);

        // set new activeLine
        activeLine.setValue(newLine);

        // cycle through vehicles and find vehicle matching old vehicle, unhighlight vehicle and it's path
        vehicles.forEach(vehicle -> {
             if (vehicle.getFullSignature().equals(oldLine)){
                vehicle.unhighlightVehicle();
            }
        });

        // cycle through vehicles and find vehicle matching new vehicle, highlight vehicle and it's path
        vehicles.forEach(vehicle -> {
            if (vehicle.getFullSignature().equals(newLine)) {
                vehicle.highlightVehicle();
                vehicleNumberString.setValue(vehicle.getVehicleNumber());
            }
        });
    }

    /**
     * Set status to default values.
     * */
    public void resetStatus(){

        vehicles.forEach(vehicle -> {
            if (vehicle.getFullSignature().equals(activeLine.getValue())){
                vehicle.unhighlightVehicle();
            }
        });
        position.setValue("-");
        lastStopProperty.setValue("-");
        nextStopProperty.setValue("-");
        activeLine.setValue("0");
        vehicleNumberString.setValue("-");
        chooseLineSelector.setValue(null);
        timeUntilNSProperty.setValue("-");

        if (displayedStreet != null){
            displayedStreet.unhighlightLine();
            displayedStreet = null;
        }

        if (displayedStop != null){
            displayedStop.unhighlightStopMark();
            displayedStop = null;
        }

    }

    /**
     * Set value of position string property.
     * @param x X value.
     * @param y Y value.
     * */
    public void setPosition(double x, double y){
        x = (int) x;
        y = (int) y;
        position.setValue("[" + x + ", " + y + "]");
    }

    /**
     * Set value of lastStopProperty string property.
     * @param stopId Stop Id.
     * */
    public void setLastStopProperty(String stopId){
        lastStopProperty.setValue(stopId);
    }

    /**
     * Set value of nextStopProperty string property.
     * @param stopId Stop Id.
     */
    public void setNextStopProperty(String stopId){
        nextStopProperty.setValue(stopId);
    }

    /**
     * Set handle of Time class.
     * @param handle Application Time class handle.
     */
    public void setTimeHandle(Time handle){
        timeHandle = handle;
    }

    /**
     * Set value of timeUntilNSProperty (time until next stop) property.
     * @param time Calculated remaining time (String).
     */
    public void setTimeUntilNSProperty(String time){
        timeUntilNSProperty.setValue(time);
    }

    /**
     * Get line number of displayed vehicle.
     * @return Displayed vehicle's line number.
     * */
    public String getActiveLine(){
        if (activeLine == null || activeLine.equals("")){
            return null;
        }
        return activeLine.get();
    }

    /**
     * Get Vehicle object of displayed vehicle.
     * @return Displayed vehicle object.
     * */
    public Vehicle getActiveVehicle(){
        if (activeLine == null || activeLine.getValue().equals("")){
            return null;
        }

        // find vehicle in list of vehicles
        for (Vehicle vehicle : vehicles){
            if (vehicle.getFullSignature().equals(activeLine.getValue())){
                return vehicle;
            }
        }
        return null;
    }

    /**
     * Get handle of Time object.
     * @return Time object.
     */
    public Time getTimeHandle(){
        return timeHandle;
    }
}

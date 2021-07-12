package controls;

import entities.Stop;
import entities.StreetMap;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.geometry.Insets;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.Separator;
import javafx.scene.layout.GridPane;

/**
 * Class StatusBarStop represents part on the status bar (fourth part from the top) displaying information about
 * specific selected stop (position, name).
 * @author Michal Rein, Ondřej Motyčka
 */
public class StatusBarStop {

    /** Handles for entire status bar and StreetMap class. */
    private GridPane stopStatusBarHandle;
    private StreetMap streetMapHandle;
    /** Object of currently displayed stop. */
    private Stop displayedStop;
    /** Labels and their bound string properties. */
    private Label stopTitle, stopPositionTitle, stopPosition;
    private StringProperty stopPositionProperty = new SimpleStringProperty("-");
    /** Choice box for choosing stops. */
    private ChoiceBox chooseStopSelector;

    /**
     * Constructor of stop part of the status bar.
     * @param statusBar Status bar handle.
     * @param sMHandle StreetMap class handle.
     */
    public StatusBarStop(GridPane statusBar, StreetMap sMHandle){
        streetMapHandle = sMHandle;
        stopStatusBarHandle = statusBar;
        // set layout
        setStatus();
    }

    /**
     * Create layout, choice box and labels. Extract required values from StreetMap class.
     */
    private void setStatus(){

        stopTitle  = new Label("Stop: ");
        stopTitle.setPadding(new Insets(0, 10, 0, 10));
        GridPane.setConstraints(stopTitle, 0, 0);

        ChoiceBox<String> chooseStop = new ChoiceBox<>();
        GridPane.setConstraints(chooseStop, 1, 0);
        this.chooseStopSelector = chooseStop;
        chooseStop.setMaxWidth(80);
        streetMapHandle.getStreetList().forEach(street -> street.getStops().forEach(stop -> chooseStopSelector.getItems().add(stop.getId())));
        chooseStop.getSelectionModel().selectedItemProperty().addListener( (v, oldStop, newStop) -> {
            streetMapHandle.getStreetList().forEach(street -> street.getStops().forEach(stop -> {
                if (stop.getId().equals(newStop)){
                    switchStop(stop);
                }
            }));
        });

        stopPositionTitle = new Label("Position: ");
        stopPositionTitle.setPadding(new Insets(0, 0, 0, 10));
        GridPane.setConstraints(stopPositionTitle, 0, 1);

        stopPosition = new Label();
        stopPosition.textProperty().bind(stopPositionProperty);
        stopPosition.setPadding(new Insets(0, 10, 0, 0));
        GridPane.setConstraints(stopPosition, 1, 1);

        Separator statusSeparator = new Separator();
        statusSeparator.setPadding(new Insets(10, 0, 0, 0));
        GridPane.setConstraints(statusSeparator, 0, 2);
        GridPane.setColumnSpan(statusSeparator, 2);

        stopStatusBarHandle.getChildren().addAll(stopTitle, chooseStop, stopPositionTitle, stopPosition, statusSeparator);

    }

    /**
     * This method is called whenever new stop is requested to be displayed on status bar.
     * Previously displayed stop must be unhighlight and the new one
     * is highlighted and displayed on status bar.
     * @param stop Stop class object that is required to be displayed.
     */
    public void switchStop(Stop stop){
        if (displayedStop != null){
            displayedStop.unhighlightStopMark();
        }
        chooseStopSelector.setValue(stop.getId());
        stop.highlightStopMark();
        stopPositionProperty.setValue("[" + stop.getCoordinate().getX() + ", " + stop.getCoordinate().getY() + "]");
        displayedStop = stop;
    }

    /**
     * Set status to default values.
     */
    public void resetStatus(){
        stopPositionProperty.setValue("-");
        chooseStopSelector.setValue(null);

        if (displayedStop != null){
            displayedStop.unhighlightStopMark();
            displayedStop = null;
        }

    }

}

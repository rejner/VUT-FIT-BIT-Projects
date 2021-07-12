package controls;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.geometry.Insets;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.Separator;
import javafx.scene.layout.GridPane;

/**
 * Class StatusBarTime represents part on the status bar (first part from the top) displaying information about
 * simulated application time. Pace of time can be set to different speeds, which can be selected by choice box.
 * @author Michal Rein, Ondřej Motyčka
 */
public class StatusBarTime {

    /** Time class handle. */
    private Time timeHadle;
    /** Status bar handle. */
    private GridPane statusBarHandle;
    /** Labels and it's string properties. */
    private Label timeTitle, time, timeSpeedTitle;
    private StringProperty timeProperty;

    /**
     * Constructor of time's status bar part, defines string properties, passes status bar handle and calls method
     * setStatus() to create layout with all the labels, buttons and choice box.
     * @param statusBar Status bar handle.
     */
    public StatusBarTime(GridPane statusBar){
        timeProperty = new SimpleStringProperty();
        statusBarHandle = statusBar;
        setStatus();
    }

    /**
     * Create layout, labels, buttons and choice box. Set default values.
     */
    private void setStatus(){

        timeTitle = new Label("Time: ");
            timeTitle.setPadding(new Insets(0, 0, 0, 10));
            GridPane.setConstraints(timeTitle, 0, 0);

        time = new Label();
            time.textProperty().bind(timeProperty);
            GridPane.setConstraints(time, 1, 0);

        timeSpeedTitle = new Label("Speed: ");
            timeSpeedTitle.setPadding(new Insets(0, 0, 0, 10));
            GridPane.setConstraints(timeSpeedTitle, 0, 1);

        ChoiceBox<String> chooseTimeSpeed = new ChoiceBox<>();
            GridPane.setConstraints(chooseTimeSpeed, 1, 1);
            chooseTimeSpeed.getItems().addAll("1x", "2x", "3x", "4x", "5x", "10x", "25x", "50x", "100x");
            chooseTimeSpeed.getSelectionModel().selectedItemProperty().addListener((v, oldSpeed, newSpeed) -> {
                switchTimeSpeed(newSpeed);
            });
            chooseTimeSpeed.setValue("1x");
            chooseTimeSpeed.setPrefWidth(80);

        Separator statusSeparator = new Separator();
        statusSeparator.setPadding(new Insets(10, 0, 0, 0));
        GridPane.setConstraints(statusSeparator, 0, 2);
        GridPane.setColumnSpan(statusSeparator, 2);

        statusBarHandle.getChildren().addAll(timeTitle, time, timeSpeedTitle, chooseTimeSpeed, statusSeparator);
    }

    /**
     * Set updated time to timeProperty String property.
     * @param newTime Updated time in HH:MM:SS format to be displayed.
     */
    public void updateTime(String newTime){
        timeProperty.setValue(newTime);
    }

    /**
     * Set new time speed ratio based on the choice of user from choice box.
     * @param newSpeed New speed ratio for pace of time.
     */
    public void switchTimeSpeed(String newSpeed){

        if (timeHadle == null){
            return;
        }

        if (newSpeed.equals("1x")){
            timeHadle.setSpeedRation(1);

        } else if (newSpeed.equals("2x")){
            timeHadle.setSpeedRation(2);

        } else if (newSpeed.equals("3x")){
            timeHadle.setSpeedRation(3);

        } else if (newSpeed.equals("4x")){
            timeHadle.setSpeedRation(4);

        } else if (newSpeed.equals("5x")){
            timeHadle.setSpeedRation(5);

        } else if (newSpeed.equals("10x")){
            timeHadle.setSpeedRation(10);

        } else if (newSpeed.equals("25x")){
            timeHadle.setSpeedRation(25);

        } else if (newSpeed.equals("50x")){
            timeHadle.setSpeedRation(50);

        } else if (newSpeed.equals("100x")){
            timeHadle.setSpeedRation(100);
        }
    }

    /**
     * Set Time class handle.
     * @param time Time class that holds information about simulated application time.
     */
    public void setTimeHandle(Time time){
        timeHadle = time;
    }

}

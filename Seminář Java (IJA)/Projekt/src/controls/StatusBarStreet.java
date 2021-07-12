package controls;

import entities.Street;
import entities.StreetMap;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.geometry.Insets;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.Separator;
import javafx.scene.layout.GridPane;

/**
 * Class StatusBarStreet represents part on the status bar (third part from the top) displaying information about
 * specific selected street (begin coordinates, end coordinates, name).
 * @author Michal Rein, Ondřej Motyčka
 */
public class StatusBarStreet {

    /** Handle for status bar. */
    private GridPane statusBarHandle;
    /** Handle for StreetMap class. */
    private StreetMap streetMapHandle;
    /** Street object of currently displayed street. */
    private Street displayedStreet;
    /** Labels and their string properties. */
    private Label streetTitle, streetStartTitle, streetStartPosition, streetEndTitle, streetEndPosition, streetCongestionTitle;
    private StringProperty streetStartProperty = new SimpleStringProperty("-");
    private StringProperty streetEndProperty = new SimpleStringProperty("-");
    private StringProperty streetCongestionLevelProperty = new SimpleStringProperty("-");
    /** Choice box for selecting streets. */
    private ChoiceBox chooseStreetSelector;
    /** Choice box for selecting congestion level. */
    private ChoiceBox chooseStreetCongestion;

    /**
     *  Constructor of street's status bar. Create and assign default values and call setStatus() method
     *  to create entire layout and set values.
     * @param statusBar Status bar handle.
     * @param sMHandle StreetMap object handle.
     */
    public StatusBarStreet(GridPane statusBar, StreetMap sMHandle){
        streetMapHandle  = sMHandle;
        statusBarHandle = statusBar;
        setStatus();
    }

    /**
     * Create layout, labels and choice box. All required values are extracted from StreetMap class.
     */
    private void setStatus(){

        streetTitle = new Label("Street: ");
        streetTitle.setPadding(new Insets(0, 10, 0, 10));
        GridPane.setConstraints(streetTitle, 0, 0);

        //Choice box for selecting streets
        ChoiceBox<String> chooseStreet = new ChoiceBox<>();
        GridPane.setConstraints(chooseStreet, 1, 0);
        this.chooseStreetSelector = chooseStreet;
        streetMapHandle.getStreetList().forEach(street -> chooseStreetSelector.getItems().add(street.getId()));
        chooseStreet.getSelectionModel().selectedItemProperty().addListener( (v, oldLine, newLine) -> {
            streetMapHandle.getStreetList().forEach(street -> {
                if (street.getId().equals(newLine)){
                    switchStreet(street);
                }
            });
        });
        chooseStreet.setMaxWidth(80);

        streetStartTitle = new Label("Start: ");
        streetStartTitle.setPadding(new Insets(0, 0, 0, 10));
        GridPane.setConstraints(streetStartTitle, 0, 2);

        streetStartPosition = new Label();
        streetStartPosition.textProperty().bind(streetStartProperty);
        streetStartPosition.setPadding(new Insets(0, 10, 0, 0));
        GridPane.setConstraints(streetStartPosition, 1, 2);

        streetEndTitle = new Label(("End: "));
        streetEndTitle.setPadding(new Insets(0, 0, 0, 10));
        GridPane.setConstraints(streetEndTitle, 0, 3);

        streetEndPosition = new Label();
        streetEndPosition.textProperty().bind(streetEndProperty);
        streetEndPosition.setPadding(new Insets(0, 10, 0, 0));
        GridPane.setConstraints(streetEndPosition, 1, 3);

        streetCongestionTitle = new Label("Congestion: ");
        streetCongestionTitle.setPadding(new Insets(0, 0, 0, 10));
        GridPane.setConstraints(streetCongestionTitle, 0, 4);

        chooseStreetCongestion = new ChoiceBox<String>();
        chooseStreetCongestion.getItems().addAll("Normal", "Slight", "Medium", "Hard");
        chooseStreetCongestion.getSelectionModel().selectedItemProperty().addListener( (v, oldVal, newVal) -> {
            if (displayedStreet != null){
                if (newVal == "Normal"){
                    displayedStreet.setCongestionLevel(0);
                } else if (newVal == "Slight"){
                    displayedStreet.setCongestionLevel(1);
                } else if (newVal == "Medium"){
                    displayedStreet.setCongestionLevel(2);
                } else if (newVal == "Hard"){
                    displayedStreet.setCongestionLevel(3);
                }
            }


        });
        chooseStreetCongestion.setPadding(new Insets(0, 10, 0, 0));
        chooseStreetCongestion.setMaxWidth(80);
        chooseStreetCongestion.setDisable(true);
        GridPane.setConstraints(chooseStreetCongestion, 1, 4);

        Separator statusSeparator = new Separator();
        statusSeparator.setPadding(new Insets(10, 0, 0,0 ));
        GridPane.setConstraints(statusSeparator, 0, 5);
        GridPane.setColumnSpan(statusSeparator, 2);

        statusBarHandle.getChildren().addAll(streetTitle, chooseStreet, streetStartTitle, streetStartPosition, streetEndTitle, streetEndPosition, streetCongestionTitle, chooseStreetCongestion, statusSeparator);
    }

    /**
     * This method is called whenever new street is requested to be displayed on status bar.
     * Previously displayed street must be unhighlight and the new one
     * is highlighted and displayed on status bar.
     * @param street New Street object to be displayed.
     */
    public void switchStreet(Street street){
        if (displayedStreet != null){
            displayedStreet.unhighlightLine();
        }
        chooseStreetSelector.setValue(street.getId());
        street.highlightLine();
        streetStartProperty.setValue("[" + street.begin().getX() + ", " + street.begin().getY() + "]");
        streetEndProperty.setValue("[" + street.end().getX() + ", " + street.end().getY() + "]");
        switch (street.getCongestionLevel()){
            case 0:
                chooseStreetCongestion.setValue("Normal");
                break;
            case 1:
                chooseStreetCongestion.setValue("Slight");
                break;
            case 2:
                chooseStreetCongestion.setValue("Medium");
                break;
            case 3:
                chooseStreetCongestion.setValue("Hard");
                break;
        }
        chooseStreetCongestion.setDisable(false);
        displayedStreet = street;
    }

    /**
     * Set status to default values.
     */
    public void resetStatus(){

        streetStartProperty.setValue("-");
        streetEndProperty.setValue("-");
        chooseStreetSelector.setValue(null);
        chooseStreetCongestion.setDisable(true);
        chooseStreetCongestion.setValue(null);

        if (displayedStreet != null){
            displayedStreet.unhighlightLine();
            displayedStreet = null;
        }

    }
}

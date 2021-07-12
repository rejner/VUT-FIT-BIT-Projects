package controls;

import entities.StreetMap;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.geometry.Insets;
import javafx.scene.control.CheckBox;
import javafx.scene.layout.VBox;

/**
 * Class StatusBarFeatures represents part on the status bar (last part) displaying check boxes providing
 * special features (display of street names and street congestion levels).
 * @author Michal Rein, Ondřej Motyčka
 */
public class StatusBarFeatures {

    /** Handle for status bar. */
    private VBox statusBarHandle;
    /** Handle for StreetMap object. */
    private StreetMap streetMapHandle;

    /**
     * Constructor of status bar part including features check boxes.
     * @param handle Status bar handle.
     * @param sMHandle StreetMap object handle.
     */
    public StatusBarFeatures(VBox handle, StreetMap sMHandle){
        statusBarHandle = handle;
        streetMapHandle = sMHandle;
        setFeatures();
    }

    /**
     * Create layout, check boxer and set events.
     */
    public void setFeatures(){

        CheckBox showCongestionCheck = new CheckBox("Show congestion");
        showCongestionCheck.setPadding(new Insets(0, 0, 0, 20));
        showCongestionCheck.selectedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if (newValue){
                    streetMapHandle.getStreetList().forEach(street -> street.showCongestionLevel());
                } else {
                    streetMapHandle.getStreetList().forEach(street -> street.hideCongestionLevel());
                }
            }
        });

        CheckBox showStreetNamesCheck = new CheckBox("Show street names");
        showStreetNamesCheck.setPadding(new Insets(0, 0, 0, 20));
        showStreetNamesCheck.selectedProperty().addListener(new ChangeListener<Boolean>() {
            @Override
            public void changed(ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) {
                if (newValue){
                    streetMapHandle.getStreetList().forEach(street -> street.showStreetNames());
                } else {
                    streetMapHandle.getStreetList().forEach(street -> street.hideStreetNames());
                }
            }
        });

        statusBarHandle.getChildren().addAll(showCongestionCheck, showStreetNamesCheck);
    }
}

package controls;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.geometry.Insets;
import javafx.geometry.Orientation;
import javafx.scene.control.Slider;
import javafx.scene.layout.VBox;

/**
 * Class ZoomSlider implements slider at the right side of the application. It's purpose is to change
 * values of map scaling. Each change of value is send to the MapPane object, that should immediately modify
 * it's scaling property.
 * @author Michal Rein, Ondřej Motyčka
 */
public class ZoomSlider {

    /** Slider handle. */
    private VBox sliderBoxHandle;
    /** MapPane handle. */
    private MapPane mapPaneHandle;

    /**
     * Constructor of ZoomSlider. Handles are set and slider created. Event of value change is set
     * to send new scaling factor to the MapPane object.
     * @param sliderBox Slider handle.
     * @param mapPane MapPane handle.
     */
    public ZoomSlider(VBox sliderBox, MapPane mapPane){
        mapPaneHandle = mapPane;
        sliderBoxHandle = sliderBox;
        Slider zoomSlider = new Slider(0.5, 2, 1);
        // zoomSlider.setMinHeight(500);
        zoomSlider.setShowTickMarks(true);
        zoomSlider.setMinorTickCount(25);
        zoomSlider.setShowTickLabels(true);
        zoomSlider.setOrientation(Orientation.VERTICAL);
        zoomSlider.setPadding(new Insets(0, 10, 0, 0));
        zoomSlider.valueProperty().addListener(new ChangeListener<Number>() {
            @Override
            public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue) {
                mapPaneHandle.setZoomFactor(newValue);
            }
        });
        sliderBoxHandle.getChildren().addAll(zoomSlider);

    }
}

package controls;

import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.event.EventHandler;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;
import javafx.scene.transform.Scale;

/**
 * MapPane is slightly modified JavaFX Pane class that implements zoom and dragging.
 * All entities are displayed on this modified pane. In the Main class, this pane is referenced as 'mapCanvas'.
 * @author Michal Rein, Ondřej Motyčka
 */
public class MapPane extends Pane {
    /** Zoom factor property (Max zoom factor is 2x, minimum is 0.5x) */
    private DoubleProperty zoomFactor = new SimpleDoubleProperty();
    /** Scale class. */
    private Scale scale;

    /** Variables for computing translation (moving) of the map.*/
    private double originalX, originalY;
    private double originalTranslateX, originalTranslateY;

    /**
     * Constructor of MapPane. Scale class is passed to the pane entity, listener which observes changes of
     * zoom factor value is set and drag event for map moving is implemented.
     */
    public MapPane(){

        // set scale property
        scale = new Scale(1, 1);
        this.getTransforms().add(scale);

        //this.setStyle("-fx-border-color: blue;"); // set border color for better visibility of map boundaries
        //this.setStyle("-fx-background-color: lightgray");
        this.setWidth(800);

        // add listener to monitor changes in zoomFactor property, calls changeZoomFactor() on factor change
        zoomFactor.addListener((observable, oldValue, newValue) -> changeZoomFactor(newValue));

        // when mouse is pressed, save X and Y of mouse position and save current translate or canvas
        this.setOnMousePressed(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                originalX = event.getSceneX();
                originalY = event.getSceneY();
                originalTranslateX = (getTranslateX());
                originalTranslateY = (getTranslateY());
            }
        });

        // on mouse drag, calculate offsets of X and Y axes, set new translate with offset added
        this.setOnMouseDragged(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                double offsetX = event.getSceneX() - originalX;
                double offsetY = event.getSceneY() - originalY;
                double newTranslateX = originalTranslateX + offsetX;
                double newTranslateY = originalTranslateY + offsetY;
                setTranslateX(newTranslateX);
                setTranslateY(newTranslateY);
            }
        });
    }

    /**
     * Set new X and Y axis scaling of the map, this can be done by modifying Scale class.
     * Method requestLayout() is invoked to request render of changes.
     * @param newValue New zoom factor value.
     */
    private void changeZoomFactor(Number newValue){
        //System.out.println("Changing scale: " + newValue);
        scale.setX(newValue.doubleValue());
        scale.setY(newValue.doubleValue());
        requestLayout();
    }

    /**
     * Set zoomFactor property value. This will be observed by listener that will call method to change overall scaling.
     * @param value Value of zoom factor.
     */
    public void setZoomFactor(Number value){
        this.zoomFactor.setValue(value);
    }
}

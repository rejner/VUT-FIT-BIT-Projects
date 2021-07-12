package entities;

import javafx.scene.effect.DropShadow;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import java.util.Objects;

/**
 * Stop class represents one unique stop on the map.
 * @author Michal Rein, Ondřej Motyčka
 */
public class Stop {

    /** Identifier. */
    private String id;
    /** Position. */
    private Coordinate position;
    /** Street object associated with a stop (will be set by a Street object when assigned to it). */
    private Street s;
    /** Stop mark Rectangle object. */
    private Rectangle stopMark;
    /** Indicator whether is stop just an auxiliary or not. */
    private boolean isAuxStop = false;

    /**
     * Constructor of a stop.
     * @param id Identifier (name) of a stop.
     * @param position Position of a stop.
     */
    public Stop(String id, Coordinate position) {
        this.id = id;
        this.position = position;
    }

    // constructor overloading
    public Stop(String id) {
        this.id = id;
        this.position = null;
    }

    /**
     * Get stop identifier (name).
     * @return Stop identifier.
     */
    public String getId(){
        return this.id;
    }

    /**
     * Get Coordinate object (position).
     * @return Stop's position on the map.
     */
    public Coordinate getCoordinate() {
        return this.position;
    }

    /**
     * Get Street object associated with a stop (stop is located on that street).
     * @return Street object where stop is located.
     */
    public Street getStreet() {
        return (this.s != null ? this.s : null);
    }

    /**
     * Get auxiliary stop indicator.
     * @return Boolean value whether stop is an auxiliary or not.
     */
    public boolean getIsAuxStop(){
        return isAuxStop;
    }

    /**
     * Set auxiliary indicator.
     */
    public void setIsAuxStop(){
        isAuxStop = true;
    }

    /**
     * Set Street object on which stop is located.
     * @param s Street object.
     */
    public void setStreet(Street s) {
        this.s = s;
    }

    /**
     * Set stop mark object.
     * @param stopRec Rectangle object.
     */
    public void setStopMark(Rectangle stopRec){
        stopMark = stopRec;
    }

    /**
     * Get stop mark object.
     * @return Rectangle object (stop's mark on the map).
     */
    public Rectangle getStopMark(){
        return stopMark;
    }

    /**
     * Highlight stop on the map.
     */
    public void highlightStopMark(){

        // resize mark
        stopMark.setWidth(12);
        stopMark.setHeight(12);
        stopMark.setX(getCoordinate().getX() - 6);
        stopMark.setY(getCoordinate().getY() - 6);

        // glow effect
        DropShadow borderGlow= new DropShadow();
        borderGlow.setOffsetY(0f);
        borderGlow.setOffsetX(0f);
        borderGlow.setColor(Color.BLUE);
        stopMark.setEffect(borderGlow);
    }

    /**
     * Unhighlight stop on the map.
     */
    public void unhighlightStopMark(){
        stopMark.setWidth(6);
        stopMark.setHeight(6);
        stopMark.setX(getCoordinate().getX() - 3);
        stopMark.setY(getCoordinate().getY() - 3);
        stopMark.setEffect(null);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Stop stop = (Stop) o;
        return Objects.equals(id, stop.id);
    }
}

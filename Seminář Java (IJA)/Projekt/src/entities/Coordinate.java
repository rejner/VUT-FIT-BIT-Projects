package entities;

/**
 * Class Coordinate is representation of real coordinate containing X and Y values.
 * It is used to describe position of certain entities (stops, beginnings of streets etc...).
 * @author Michal Rein, Ondřej Motyčka
 */
public class Coordinate {

    /** X value. */
    private int x;
    /** Y value. */
    private int y;

    /**
     * Creator of Coordinate class. Check for valid coordinate is performed (can't have negative values).
     * @param x X value.
     * @param y Y value.
     * @return Valid Coordinate object.
     */
    public static Coordinate create(int x, int y) {

        Coordinate c = new Coordinate();
        if (x > 0 && y > 0) {
            c.setCoordinates(x, y);
            return c;
        }
        return null;
    }

    /**
     * Set X and Y values to Coordinate object.
     * @param x X value.
     * @param y Y value.
     */
    public void setCoordinates(int x, int y){
        this.x = x;
        this.y = y;
    }

    /**
     * Get X value of Coordinate object.
     * @return X value.
     */
    public int getX(){
        return this.x;
    }

    /**
     * Get Y value of Coordinate object.
     * @return Y value.
     */
    public int getY(){
        return this.y;
    }

    /**
     * Calculate X value difference between current and given Coordinate objects.
     * @param c Coordinate to perform difference operation with.
     * @return Calculated difference of X values of two Coordinate objects.
     */
    public int diffX(Coordinate c) {
        return this.x - c.getX();
    }

    /**
     * Calculate Y value difference between current and given Coordinate objects.
     * @param c Coordinate to perform difference operation with.
     * @return Calculated difference of Y values of two Coordinate objects.
     */
    public int diffY(Coordinate c) {
        return this.y - c.getY();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Coordinate that = (Coordinate) o;
        return x == that.x &&
                y == that.y;
    }
}

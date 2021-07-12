package vut.fit.ija.homework1.myMaps;
import vut.fit.ija.homework1.maps.Coordinate;

import java.util.Objects;

public class MyCoordinate implements Coordinate{

    private int x;
    private int y;

    public static Coordinate create(int x, int y) {

        MyCoordinate c = new MyCoordinate();
        if (x > 0 && y > 0) {
            c.setCoordinates(x, y);
            return c;
        }
        return null;
    }

    public void setCoordinates(int x, int y){
        this.x = x;
        this.y = y;
    }

    public int getX(){
        return this.x;
    }

    public int getY(){
        return this.y;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        MyCoordinate that = (MyCoordinate) o;
        return x == that.x &&
                y == that.y;
    }
}

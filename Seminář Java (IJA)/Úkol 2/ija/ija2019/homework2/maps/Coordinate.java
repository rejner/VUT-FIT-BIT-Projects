package ija.ija2019.homework2.maps;

public class Coordinate {

    private int x;
    private int y;

    public static Coordinate create(int x, int y) {

        Coordinate c = new Coordinate();
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

    public int diffX(Coordinate c) {
        return this.x - c.getX();
    }

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

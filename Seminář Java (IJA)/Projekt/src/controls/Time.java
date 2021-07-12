package controls;

import entities.Street;

/**
 * Time class holds information about simulated application time.
 * It also provides useful methods for work with simulated time and constructing time tables.
 * @author Michal Rein, Ondřej Motyčka
 */
public final class Time {

    /** Represents current simulated time in seconds. */
    long appTime = 0;
    /** Speed ratio, increasing this ration increases pace of time. */
    int speedRatio = 1;
    /** Delay counter. Delay is affected by speed ratio.*/
    int delayCounter = 0;
    /** Handle for status bar. */
    StatusBarTime timeStatusBarHandle;

    /**
     * Constructor of Time class. Sets current time to starting time given by parameter start.
     * @param start Starting value of time.
     * @param timeStatusBar Status bar handle.
     */
    public Time(int start, StatusBarTime timeStatusBar){
        appTime = start;
        timeStatusBarHandle = timeStatusBar;
    }

    /**
     * Simulate pass of one second in simulated world. After trespassing value of 86400 (which stands for 24 hours),
     * time must be reset. Status bar is also informed about time change and new value is displayed.
     */
    public void updateTime(){
        appTime++; // increment simulated application time
        if (appTime >= 86400) appTime = 0;  // after 24 hours, timer must be reset
        // update time on status bar
        timeStatusBarHandle.updateTime(convertTimeToString(appTime));
    }

    /**
     * Get current simulated time.
     * @return Simulated time (in seconds).
     */
    public long getAppTime(){
        return appTime;
    }

    /**
     * Reset of delay counter.
     */
    public void resetDelayCounter(){
        delayCounter = 0;
    }

    /**
     * Incrementation of delay counter.
     */
    public void incrementDelayCounter(){
        delayCounter++;
    }

    /**
     * Get delay counter.
     * @return Delay counter value.
     */
    public int getDelayCounter(){
        return delayCounter;
    }

    /**
     * Get speed ratio.
     * @return Speed ratio value.
     */
    public int getSpeedRatio(){
        return speedRatio;
    }

    /**
     * Set new speed ratio.
     * @param newSpeed New value of speed ratio.
     */
    public void setSpeedRation(int newSpeed){
        speedRatio = newSpeed;
    }

    /**
     * Method converts time given in seconds into HH:MM:SS format.
     * @param time Time which has to be converted into readable format.
     * @return Time in HH:MM:SS format.
     */
    public static String convertTimeToString(long time){
        long hours = time / 3600;
        long minutes = (time / 60) % 60;
        long secs = time % 60;
        return String.format("%02d", hours) + ":" + String.format("%02d", minutes) + ":" + String.format("%02d", secs);
    }

    /**
     * Method converts time given in HH:MM:SS format into seconds.
     * @param timeString Time in HH:MM:SS format.
     * @return Time in seconds.
     */
    public static long convertStringToTime(String timeString){
        long time = 0;
        String[] parts = timeString.split(":");
        time += Integer.parseInt(parts[0]) * 3600;
        time += Integer.parseInt(parts[1]) * 60;
        time += Integer.parseInt(parts[2]);
        return time;
    }

    /**
     * Calculate time difference between current time and given time.
     * @param time Time is seconds.
     * @return Difference of current time and time given by parameter.
     */
    public String calculateTimeDifference(long time){
        long newTime = time - appTime;
        return convertTimeToString(newTime);
    }

    /**
     * Return congestion level constant. It is used for calculation of estimated time arrival.
     * @param street Street from which we need to get congestion constant.
     * @return Congestion constant of given street.
     */
    public static int getCongestionConstant(Street street){
        int constant;
        switch (street.getCongestionLevel()){
            case 1:
                constant = 2;
                break;
            case 2:
                constant = 4;
                break;
            case 3:
                constant = 10;
                break;
            default:
                constant = 1;
                break;
        }
        return constant;
    }

}

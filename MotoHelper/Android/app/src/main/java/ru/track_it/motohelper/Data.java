package ru.track_it.motohelper;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Data {

    public static final int samplePeriod = 10;
    public static final int accelDataPointsLimit = 1000;    //10s
    public static final int pointsCountToShowOnGraph = 100; //1s

    public static final int maxAccel = 512;
    public static final int minAccel = -maxAccel;


    public static final Map<Long, AccelData> accelData = new HashMap<>();
    public static final List<AccelData> accelArray=new ArrayList<>();

    public static Comparator<AccelData> accelDataSorter = new Comparator<AccelData>() {
        @Override
        public int compare(AccelData d1, AccelData d2) {
            if (d1.ms > d2.ms) {
                return 1;
            } else if (d2.ms > d1.ms) {
                return -1;
            }
            return 0;
        }
    };
}

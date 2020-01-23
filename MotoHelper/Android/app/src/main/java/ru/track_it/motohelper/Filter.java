package ru.track_it.motohelper;

public class Filter {

    private int beating = 10;
    private int minimumPointsForAnalize;
    private long  lastAnalizedTimestamp = 0;
    private int samplePeriod;

    public Filter(){
        samplePeriod = Data.samplePeriod;
        minimumPointsForAnalize = Data.pointsCountToShowOnGraph-20;

    }

    public static class AccelerationResult{
        int value;
        long lastMS;
    }

    /**
     * @param data отсортированные данные
     */
    AccelerationResult analizeData(AccelData[] data, long previousTargetMs){
        if (data == null || data.length<minimumPointsForAnalize){
            return null;
        }
        int previousTargetIndex = 0;
        if (previousTargetMs!=0){
            for (int i = 0; i<data.length;i++){
                if (data[i].ms>=previousTargetMs)
                {
                    previousTargetIndex = i;
                    break;
                }
            }
        }

        if (data.length-previousTargetIndex<minimumPointsForAnalize){
            return null;
        }

        int sixth = minimumPointsForAnalize/6;
        int firstIndexForAnalyze=data.length-sixth*3;
        int lastIndexForAnalyze = data.length-1;

        for (int i=lastIndexForAnalyze;i>=0;i--){
            AccelData accelData = data[i];
            if (accelData.ms<=lastAnalizedTimestamp){
                firstIndexForAnalyze = i;
                break;
            }
        }
        //количество рассматриваемых точек
        int elementsToAnalyze = data.length - firstIndexForAnalyze;
        if (elementsToAnalyze<minimumPointsForAnalize){
            return;
        }

        //min & max в первой трети анализируемой трети
        int min1 = data[firstIndexForAnalyze].x;
        int max1 = data[firstIndexForAnalyze].x;
        int end = firstIndexForAnalyze + sixth;
        for (int i = firstIndexForAnalyze; i<end;i++){
            int x = data[i].x;
            if (x<min1){
                min1=x;
            }
            if (x>max1){
                max1=x;
            }
        }
        end = lastIndexForAnalyze - sixth;
        int min3 = data[lastIndexForAnalyze].x;
        int max3 = data[lastIndexForAnalyze].x;
        for (int i = lastIndexForAnalyze; i>end;i--){
            int x = data[i].x;
            if (x<min3){
                min3=x;
            }
            if (x>max3){
                max3=x;
            }
        }

        int avg1 = (max1-min1)/2;
        int avg3 = (max3-min3)/2;
        int avg2 = (avg3-avg1)/2;

        //ищем точку ближайшую с реднему значению
        int differ = Math.abs(data[lastIndexForAnalyze].x-avg2);
        int targetIndex = lastIndexForAnalyze;
        for (int i = lastIndexForAnalyze; i>firstIndexForAnalyze;i--){
            int nextDiffer = Math.abs(data[i].x-avg2);
            if (nextDiffer < differ){
                differ = nextDiffer;
                targetIndex = i;
            }
        }

        //к этому моменту мы имеем искомый средний индекс
        int avgX=0;
        int count = targetIndex-previousTargetIndex+1;
        for (int i = previousTargetIndex; i<=targetIndex;i++){
            avgX+=data[i].x;
        }
        avgX/=count;

        AccelerationResult result = new AccelerationResult();
        result.value = avgX;
        result.lastMS = data[targetIndex].ms;
        return result;
    }


}

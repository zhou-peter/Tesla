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


    /**
     * @param data отсортированные данные
     */
    void analizeData(AccelData[] data){
        if (data == null || data.length<minimumPointsForAnalize){
            return;
        }
        int firstIndexForAnalyze=0;
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

        int vSearchStart = lastIndexForAnalyze-(elementsToAnalyze/3);
        int vSearchStop = lastIndexForAnalyze-(elementsToAnalyze/4);
        
        int sum = 0;
        //в 25 процентной зоне спрва ищем точку тишины
        for (int i=firstIndexForAnalyze;i<=lastIndexForAnalyze;i++){
            sum+=data[i].x;
        }
        sum/=elementsToAnalyze;
        if (Math.abs(sum)<=beating){
            // не произошло ничего
            lastAnalizedTimestamp = data[data.length-1].ms;
            return;
        }
        //ищем точку в)


    }


}

package ru.track_it.motohelper;

import androidx.arch.core.util.Function;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Transformations;
import androidx.lifecycle.ViewModel;


import com.github.mikephil.charting.data.Entry;

import java.util.ArrayList;
import java.util.List;

import static ru.track_it.motohelper.Data.accelArray;
import static ru.track_it.motohelper.Data.accelLock;

public class CalibrationViewModel extends ViewModel {
    private MutableLiveData<List<AccelData>> graphDataSource = new MutableLiveData<>();

    private LiveData<ThreeAxisData> graphData = Transformations.map(graphDataSource, new Function<List<AccelData>, ThreeAxisData>() {
        @Override
        public ThreeAxisData apply(List<AccelData> input) {
            List<AccelData> data = null;
            synchronized (accelLock) {
                data=new ArrayList<>(input.size());
                data.addAll(input);
            }
            ThreeAxisData result=new ThreeAxisData();
            int index = 0;
            if (data.size()>0){
                if (data.size()>Data.pointsCountToShowOnGraph){
                    index = data.size()-Data.pointsCountToShowOnGraph;
                }
                int step = 0;
                for(;index<data.size();index++){
                    AccelData ad=data.get(index);
                    result.xAxisData.add(new Entry(step, ad.x));
                    result.yAxisData.add(new Entry(step, ad.y));
                    result.zAxisData.add(new Entry(step, ad.z));
                    step++;
                }
            }
            return result;
        }
    });

    public LiveData<ThreeAxisData> getGraphData(){
        return graphData;
    }

    public void updateData(List<AccelData> newData){
        graphDataSource.postValue(newData);

    }
}

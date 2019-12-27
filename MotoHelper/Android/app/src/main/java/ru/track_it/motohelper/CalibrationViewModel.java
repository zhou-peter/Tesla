package ru.track_it.motohelper;

import androidx.arch.core.util.Function;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Transformations;
import androidx.lifecycle.ViewModel;


import com.github.mikephil.charting.data.Entry;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;



public class CalibrationViewModel extends ViewModel {
    private MutableLiveData<List<AccelData>> graphDataSource = new MutableLiveData<>();

    private LiveData<List<AccelData>> graphData = Transformations.map(graphDataSource, new Function<List<AccelData>, List<AccelData>>() {
        @Override
        public List<AccelData> apply(List<AccelData> input) {
            return input;
        }
    });

    public LiveData<List<AccelData>> getGraphData(){
        return graphData;
    }

    public void updateData(List<AccelData> newData){
        graphDataSource.postValue(newData);
    }
}

package ru.track_it.motohelper;

import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import android.graphics.Canvas;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.os.Looper;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;




import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;

import ru.track_it.motohelper.Graph.GraphCanvas;
import ru.track_it.motohelper.Graph.GraphDataProvider;


public class Calibration extends Fragment {

    private CalibrationViewModel mViewModel;
    View root;
    boolean canRun=true;
    GraphCanvas graph;
    List<AccelData> accelData=new ArrayList<>();
    MyGraphDataProvider dataProvider=new MyGraphDataProvider();




    public static Calibration newInstance() {
        return new Calibration();
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        root = inflater.inflate(R.layout.calibration_fragment, container, false);
        graph = (GraphCanvas) root.findViewById(R.id.graph);

        graph.setSeriesCount(3);
        graph.setDataProvider(dataProvider);

        for (int i=0; i<Data.pointsCountToShowOnGraph+1; i++){
            AccelData ad = new AccelData();
            ad.x=i;
            ad.y=-i;
            ad.z=i*i;
            accelData.add(ad);
        }

        new Thread(dataProvider).start();

        return root;
    }



    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders.of(this).get(CalibrationViewModel.class);
        mViewModel.getGraphData().removeObservers(this);
        mViewModel.getGraphData().observe(this, new Observer<List<AccelData>>() {
            @Override
            public synchronized void onChanged(List<AccelData> data) {
                accelData = data;
            }
        });
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        canRun = false;
    }


    public CalibrationViewModel getModel() {
        return mViewModel;
    }



    private class MyGraphDataProvider implements GraphDataProvider, Runnable {

        //MEM pages for drawing
        int[] xData=new int[Data.pointsCountToShowOnGraph];
        int[] yData=new int[Data.pointsCountToShowOnGraph];
        int[] zData=new int[Data.pointsCountToShowOnGraph];

        @Override
        public int getItemsCount() {
            return Data.pointsCountToShowOnGraph;
        }

        @Override
        public int getItemValue(int seriesIndex, int index) {
            switch (seriesIndex) {
                case 0:
                    return xData[index];
                case 1:
                    return yData[index];
                case 2:
                    return zData[index];
            }
            return 0;
        }

        @Override
        public int getMaxValue() {
            return Data.maxAccel;
        }

        @Override
        public int getMinValue() {
            return Data.minAccel;
        }

        @Override
        public int getSeriesColor(int seriesIndex) {
            switch (seriesIndex){
                case 0:
                    return 0xFFFF0000;
                case 1:
                    return 0xFF00FF00;
                case 2:
                    return 0xFF0000FF;
            }
            return 0xFFFFFFFF;
        }

        Runnable redrawer=new Runnable() {
            @Override
            public void run() {
                graph.invalidate();
            }
        };

        @Override
        public void run() {
            while(canRun){

                try {
                    final List<AccelData> data = accelData;
                    if (data!=null){
                        int newDataSize = data.size();
                        if (newDataSize >= Data.pointsCountToShowOnGraph) {
                            int stopIndex = newDataSize - Data.pointsCountToShowOnGraph;
                            for (int i = newDataSize-1, j=Data.pointsCountToShowOnGraph-1;
                                 i>=stopIndex; i--, j--){
                                AccelData accelData=data.get(i);
                                xData[j]=accelData.x;
                                yData[j]=accelData.y;
                                zData[j]=accelData.z;
                            }
                            Executors.MainThreadExecutor.execute(redrawer);
                        }
                    }
                    Thread.sleep(30);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

}

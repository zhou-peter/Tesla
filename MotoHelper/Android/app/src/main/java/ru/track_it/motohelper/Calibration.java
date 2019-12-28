package ru.track_it.motohelper;

import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import android.graphics.Color;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;

import java.util.ArrayList;
import java.util.List;


public class Calibration extends Fragment {

    private CalibrationViewModel mViewModel;
    View root;
    private long lastTimestamp = 0;
    LineChart graph;
    LineDataSet xAxisDataSet, yAxisDataSet, zAxisDataSet;
    List<Entry> xData=new ArrayList<>();
    List<Entry> yData=new ArrayList<>();
    List<Entry> zData=new ArrayList<>();

    public static Calibration newInstance() {
        return new Calibration();
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        root = inflater.inflate(R.layout.calibration_fragment, container, false);
        graph = (LineChart) root.findViewById(R.id.graph);
        //graph.animateXY(3000,3000);
        graph.setViewPortOffsets(0,0,0,0);


        xAxisDataSet =new LineDataSet(xData, "X");
        xAxisDataSet.setColor(Color.MAGENTA);
        yAxisDataSet =new LineDataSet(yData, "Y");
        yAxisDataSet.setColor(Color.rgb(255,128, 0));//ORANGE
        zAxisDataSet =new LineDataSet(zData, "Z");
        zAxisDataSet.setColor(Color.BLUE);

        LineData data=new LineData(xAxisDataSet, yAxisDataSet, zAxisDataSet);
        graph.setData(data);

        return root;//inflater.inflate(R.layout.calibration_fragment, container, false);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders.of(this).get(CalibrationViewModel.class);
        mViewModel.getGraphData().removeObservers(this);
        mViewModel.getGraphData().observe(this, new Observer<List<AccelData>>() {
            @Override
            public void onChanged(List<AccelData> threeAxisData) {
                //add new data
                for (AccelData ac : threeAxisData){
                    if (ac.ms>lastTimestamp){
                        xData.add(new Entry(ac.ms, ac.x));
                        yData.add(new Entry(ac.ms, ac.y));
                        zData.add(new Entry(ac.ms, ac.z));
                        lastTimestamp=ac.ms;
                    }
                }
                //clear old
                while (xData.size()>Data.pointsCountToShowOnGraph){
                    xData.remove(0);
                    yData.remove(0);
                    zData.remove(0);
                }

                xAxisDataSet.notifyDataSetChanged();
                yAxisDataSet.notifyDataSetChanged();
                zAxisDataSet.notifyDataSetChanged();
                graph.notifyDataSetChanged();
                graph.invalidate();
            }
        });
    }

    public CalibrationViewModel getModel(){
        return mViewModel;
    }

}

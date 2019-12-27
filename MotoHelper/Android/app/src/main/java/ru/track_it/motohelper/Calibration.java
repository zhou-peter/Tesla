package ru.track_it.motohelper;

import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import android.graphics.Color;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.os.Looper;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;


public class Calibration extends Fragment {

    private CalibrationViewModel mViewModel;
    View root;
    LineChart graph;


    public static Calibration newInstance() {
        return new Calibration();
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        root = inflater.inflate(R.layout.calibration_fragment, container, false);
        graph = (LineChart) root.findViewById(R.id.graph);
        graph.animateXY(3000,3000);
        //add empty data
        graph.setData(new LineData());
        graph.setViewPortOffsets(0,0,0,0);

        return root;//inflater.inflate(R.layout.calibration_fragment, container, false);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders.of(this).get(CalibrationViewModel.class);
        mViewModel.getGraphData().removeObservers(this);
        mViewModel.getGraphData().observe(this, new Observer<ThreeAxisData>() {
            @Override
            public void onChanged(ThreeAxisData threeAxisData) {
                LineDataSet xAxis=new LineDataSet(threeAxisData.xAxisData, "X");
                xAxis.setColor(Color.MAGENTA);
                LineDataSet yAxis=new LineDataSet(threeAxisData.yAxisData, "Y");
                yAxis.setColor(Color.rgb(255,128, 0));//ORANGE
                LineDataSet zAxis=new LineDataSet(threeAxisData.zAxisData, "Z");
                zAxis.setColor(Color.BLUE);

                LineData data=new LineData(xAxis, yAxis, zAxis);

                graph.setData(data);

                graph.notifyDataSetChanged();
                graph.invalidate();
            }
        });
    }

    public CalibrationViewModel getModel(){
        return mViewModel;
    }

}

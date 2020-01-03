package ru.track_it.motohelper;

import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import android.graphics.Canvas;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.androidplot.Plot;
import com.androidplot.PlotListener;
import com.androidplot.xy.BoundaryMode;
import com.androidplot.xy.LineAndPointFormatter;
import com.androidplot.xy.XYPlot;
import com.androidplot.xy.XYSeries;


import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;


public class Calibration extends Fragment {

    private CalibrationViewModel mViewModel;
    View root;

    XYPlot graph;
    //unmodifiable list
    List<AccelData> staticGraphData = new ArrayList<>();


    SampleDynamicSeries xSeries = new SampleDynamicSeries(Axis.X, "X");
    SampleDynamicSeries ySeries = new SampleDynamicSeries(Axis.Y, "Y");
    SampleDynamicSeries zSeries = new SampleDynamicSeries(Axis.Z, "Z");
    ReentrantLock graphDataModifierLock=new ReentrantLock();



    public static Calibration newInstance() {
        return new Calibration();
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        root = inflater.inflate(R.layout.calibration_fragment, container, false);
        graph = (XYPlot) root.findViewById(R.id.graph);


        LineAndPointFormatter seriesFormatX =
                new LineAndPointFormatter(root.getContext(), R.xml.line_format_x);
        LineAndPointFormatter seriesFormatY =
                new LineAndPointFormatter(root.getContext(), R.xml.line_format_y);
        LineAndPointFormatter seriesFormatZ =
                new LineAndPointFormatter(root.getContext(), R.xml.line_format_z);
        graph.addSeries(xSeries, seriesFormatX);
        graph.addSeries(ySeries, seriesFormatY);
        graph.addSeries(zSeries, seriesFormatZ);

        graph.setRangeBoundaries(Data.minAccel, Data.maxAccel, BoundaryMode.FIXED);
        graph.addListener(graphListener);

        return root;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders.of(this).get(CalibrationViewModel.class);
        mViewModel.getGraphData().removeObservers(this);
        mViewModel.getGraphData().observe(this, new Observer<List<AccelData>>() {
            /**
             * @param threeAxisData
             */
            @Override
            public void onChanged(List<AccelData> threeAxisData) {
                int newDataSize = threeAxisData.size();
                if (newDataSize > 0) {
                    if (graphDataModifierLock.tryLock()) {
                        staticGraphData = threeAxisData;
                        graphDataModifierLock.unlock();
                        graph.redraw();
                    }
                }
            }
        });
    }

    PlotListener graphListener=new PlotListener() {
        @Override
        public void onBeforeDraw(Plot source, Canvas canvas) {
            graphDataModifierLock.lock();
        }

        @Override
        public void onAfterDraw(Plot source, Canvas canvas) {
            graphDataModifierLock.unlock();
        }
    };


    public CalibrationViewModel getModel() {
        return mViewModel;
    }

    enum Axis {
        X,
        Y,
        Z,
    }

    class SampleDynamicSeries implements XYSeries {


        private Axis accelAxis;
        private String title;

        SampleDynamicSeries(Axis seriesIndex, String title) {
            this.accelAxis = seriesIndex;
            this.title = title;
        }

        @Override
        public String getTitle() {
            return title;
        }

        @Override
        public int size() {
            return staticGraphData.size();
        }

        @Override
        public Number getX(int index) {
            return index;
        }

        @Override
        public Number getY(int index) {
            if (index>=staticGraphData.size()){
                return 0;
            }
            AccelData sample = staticGraphData.get(index);
            switch (accelAxis) {
                case X:
                    return sample.x;
                case Y:
                    return sample.y;
                case Z:
                    return sample.z;
            }
            return 0;
        }
    }
}

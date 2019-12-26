package ru.track_it.motohelper;

import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.jjoe64.graphview.GraphView;

public class Calibration extends Fragment {

    private CalibrationViewModel mViewModel;
    View root;
    GraphView graph;
    public static Calibration newInstance() {
        return new Calibration();
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {

        root = inflater.inflate(R.layout.calibration_fragment, container, false);
        graph = (GraphView) root.findViewById(R.id.graph);
        //graph.addSeries(series);
        //graph.getViewport().setXAxisBoundsManual(true);
        //graph.getGridLabelRenderer().setLabelFormatter(this);
        return inflater.inflate(R.layout.calibration_fragment, container, false);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders.of(this).get(CalibrationViewModel.class);
        mViewModel.getGraphData().observe(this, new Observer<ThreeAxisData>() {
            @Override
            public void onChanged(ThreeAxisData threeAxisData) {
                graph.removeAllSeries();
                graph.addSeries(threeAxisData.xAxisData);
                graph.addSeries(threeAxisData.yAxisData);
                graph.addSeries(threeAxisData.zAxisData);
            }
        });
    }

}

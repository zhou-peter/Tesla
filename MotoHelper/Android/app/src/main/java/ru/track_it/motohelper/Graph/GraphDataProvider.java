package ru.track_it.motohelper.Graph;

import android.graphics.Color;

public interface GraphDataProvider {
    //сколько показать на графике
    int getItemsCount();
    //получить Y значение для какой либо оси
    int getItemValue(int seriesIndex, int index);

    int getMaxValue();
    int getMinValue();

    int getSeriesColor(int seriesIndex);
}

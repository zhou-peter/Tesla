package ru.track_it.motohelper.Graph;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;

import androidx.annotation.Nullable;

public class GraphCanvas extends View {

    private int seriesCount = 0;
    private GraphDataProvider dataProvider;
    private Paint paint=new Paint();
    // -\|/
    private int spinnerCounter=0;

    public GraphCanvas(Context context) {
        super(context);
        initGraph();
    }

    public GraphCanvas(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        initGraph();
    }

    public GraphCanvas(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initGraph();
    }

    private void initGraph(){
        paint.setStrokeWidth(2);
        paint.setAntiAlias(true);
        paint.setTextSize(30);
    }

    public void setSeriesCount(int count) {
        this.seriesCount = count;
    }

    public void setDataProvider(GraphDataProvider dataProvider) {
        this.dataProvider = dataProvider;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (dataProvider == null) {
            return;
        }
        drawSpinner(canvas);


        int width = getWidth();
        int height = getHeight();
        int minValue = dataProvider.getMinValue();
        int dataHeight = dataProvider.getMaxValue() - minValue;
        int dataWidth = dataProvider.getItemsCount();
        float yScale = (float) height / (float) dataHeight;
        float xScale = (float) width / (float) dataWidth;


        if (dataHeight > 0 && dataWidth > 0 && seriesCount > 0) {
            for (int seriesIndex=0; seriesIndex< seriesCount;seriesIndex++){
                float prevX=0;
                float prevY= yRecalculate(yScale,height, minValue, dataProvider.getItemValue(seriesIndex, 0));

                paint.setColor(dataProvider.getSeriesColor(seriesIndex));


                for (int index = 1;index<dataWidth;index++){
                    float x=xRecalculate(xScale, index);
                    float y =yRecalculate(yScale, height, minValue, dataProvider.getItemValue(seriesIndex, index));
                    canvas.drawLine(prevX, prevY, x, y, paint);
                    prevX = x;
                    prevY = y;
                }
            }
        }
    }

    private static float yRecalculate(float scale, float height, float minValue, float value){
        //delta from bottom
        value = value - minValue;
        value*=scale;
        //convert to from-top
        value = height-value;
        return value;
    }
    private static float xRecalculate(float scale, float index){
        return index*scale;
    }


    private  void drawSpinner(Canvas canvas)
    {
        int offset = 30;

        switch ( spinnerCounter){
            case 0:
                canvas.drawText("-",offset, offset, paint);
            case 1:
                canvas.drawText("\\",offset, offset, paint);
            case 2:
                canvas.drawText("/",offset, offset, paint);
        }
        spinnerCounter++;
        if (spinnerCounter>=3){
            spinnerCounter=0;
        }
    }
}

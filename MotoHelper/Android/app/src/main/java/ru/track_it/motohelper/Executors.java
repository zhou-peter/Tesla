package ru.track_it.motohelper;

import android.os.Handler;
import android.os.Looper;

import java.util.concurrent.Executor;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class Executors {

    public static final Executor MainThreadExecutor =new Executor() {
        private final Handler handler = new Handler(Looper.getMainLooper());
        @Override
        public void execute(Runnable command) {
            handler.post(command);
        }
    };

    public static final ThreadPoolExecutor BackGroundThreadPool=new ThreadPoolExecutor(3,5,600,
            TimeUnit.MILLISECONDS, new LinkedBlockingQueue<Runnable>());

}

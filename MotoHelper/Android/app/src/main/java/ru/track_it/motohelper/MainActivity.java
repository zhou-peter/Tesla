package ru.track_it.motohelper;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.tabs.TabLayout;

import androidx.annotation.NonNull;
import androidx.viewpager.widget.ViewPager;
import androidx.appcompat.app.AppCompatActivity;

import android.util.Log;
import android.view.View;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import permissions.dispatcher.NeedsPermission;
import permissions.dispatcher.OnShowRationale;
import permissions.dispatcher.PermissionRequest;
import permissions.dispatcher.RuntimePermissions;
import ru.track_it.motohelper.Packets.AbstractInPacket;
import ru.track_it.motohelper.Packets.PacketsListener;
import ru.track_it.motohelper.Packets.PacketsManager;
import ru.track_it.motohelper.Packets.Utils;
import ru.track_it.motohelper.ui.main.SectionsPagerAdapter;

import static ru.track_it.motohelper.Data.accelArray;
import static ru.track_it.motohelper.Data.accelData;
import static ru.track_it.motohelper.Data.accelDataPointsLimit;

@RuntimePermissions
public class MainActivity extends AppCompatActivity {

    CommunicationManagerBLE communicationManager = new CommunicationManagerBLE(this);
    PacketsManager packetsManager;
    private final static String LOG_TAG = "MainActivity";
    AtomicBoolean packetsProcessing = new AtomicBoolean(false);
    SectionsPagerAdapter sectionsPagerAdapter;

    private boolean bluetoothGranded = false;
    private boolean bluetoothAdminGranded = false;
    private boolean coarsetoothGranded = false;
    private boolean fineLocationGranded = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        sectionsPagerAdapter = new SectionsPagerAdapter(this,
                getSupportFragmentManager());
        ViewPager viewPager = findViewById(R.id.view_pager);
        viewPager.setAdapter(sectionsPagerAdapter);
        TabLayout tabs = findViewById(R.id.tabs);
        tabs.setupWithViewPager(viewPager);


        //FloatingActionButton fab = findViewById(R.id.fab);


        Executors.BackGroundThreadPool.execute(new Runnable() {
            @Override
            public void run() {
                while (true) {
                    try {
                        Thread.sleep(2000);
                        if (!coarsetoothGranded) {
                            MainActivityPermissionsDispatcher.grantedCoarseLocationWithPermissionCheck(MainActivity.this);
                        } else if (!bluetoothGranded) {
                            MainActivityPermissionsDispatcher.grantedBluetoothWithPermissionCheck(MainActivity.this);
                        } else if (!bluetoothAdminGranded) {
                            MainActivityPermissionsDispatcher.grantedBluetoothAdminWithPermissionCheck(MainActivity.this);
                        } else if (!fineLocationGranded) {
                            MainActivityPermissionsDispatcher.grantedFineLocationWithPermissionCheck(MainActivity.this);
                        }
                        if (coarsetoothGranded && fineLocationGranded &&
                                bluetoothGranded && bluetoothAdminGranded && BluetoothAdapter.getDefaultAdapter().isEnabled()) {
                            createCommunicationInstance();
                            break;
                        }
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                        break;
                    }

                }
            }
        });


    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        MainActivityPermissionsDispatcher.onRequestPermissionsResult(this, requestCode, grantResults);
    }

    @OnShowRationale(Manifest.permission.ACCESS_FINE_LOCATION)
    void showPermissionMessageFINE(final PermissionRequest request) {
        new AlertDialog.Builder(this)
                .setTitle("Permission needed")
                .setMessage("If you want to use this application, you must accept")
                .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        request.proceed();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        request.cancel();
                    }
                }).create().show();
    }

    @NeedsPermission(Manifest.permission.ACCESS_FINE_LOCATION)
    void grantedFineLocation() {
        fineLocationGranded = true;
    }

    @OnShowRationale(Manifest.permission.ACCESS_COARSE_LOCATION)
    void showPermissionMessageCOARSE(final PermissionRequest request) {
        new AlertDialog.Builder(this)
                .setTitle("Permission needed")
                .setMessage("If you want to use this application, you must accept")
                .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        request.proceed();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        request.cancel();
                    }
                }).create().show();
    }

    @NeedsPermission(Manifest.permission.ACCESS_COARSE_LOCATION)
    void grantedCoarseLocation() {
        coarsetoothGranded = true;
    }

    @OnShowRationale(Manifest.permission.BLUETOOTH)
    void showPermissionMessageBluetooth(final PermissionRequest request) {
        new AlertDialog.Builder(this)
                .setTitle("Permission needed")
                .setMessage("If you want to use this application, you must accept")
                .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        request.proceed();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        request.cancel();
                    }
                }).create().show();
    }

    @NeedsPermission(Manifest.permission.BLUETOOTH)
    void grantedBluetooth() {
        bluetoothGranded = true;
    }

    @OnShowRationale(Manifest.permission.BLUETOOTH_ADMIN)
    void showPermissionMessageBluetoothAdmin(final PermissionRequest request) {
        new AlertDialog.Builder(this)
                .setTitle("Permission needed")
                .setMessage("If you want to use this application, you must accept")
                .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        request.proceed();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        request.cancel();
                    }
                }).create().show();
    }

    @NeedsPermission(Manifest.permission.BLUETOOTH_ADMIN)
    void grantedBluetoothAdmin() {
        bluetoothAdminGranded = true;
        if (!BluetoothAdapter.getDefaultAdapter().isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, 1);
        }
    }


    void createCommunicationInstance() {

        Executors.BackGroundThreadPool.execute(new Runnable() {
            @Override
            public void run() {

                communicationManager.Connect();
                while (!communicationManager.isReady()) {
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                if (communicationManager.isReady()) {
                    packetsManager = new PacketsManager(communicationManager.getInputStream(),
                            communicationManager.getOutputStream(), pl);
                }
            }
        });


    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        communicationManager.closeSocket();
    }

    PacketsListener pl = new PacketsListener() {
        @Override
        public void onNewPacketsCame() {
            if (packetsProcessing.compareAndSet(false, true)) {
                try {
                    Calibration calibrationFragment = (Calibration) sectionsPagerAdapter.getItem(0);
                    while (!packetsManager.receivedPackets.isEmpty()) {
                        AbstractInPacket packet = packetsManager.receivedPackets.poll();
                        switch (packet.Command) {
                            case 0x01:
                                break;
                            default:
                                packet.DefaultProcess();
                        }
                    }
                    //create temprory sorted list
                    List<AccelData> tmpList = new ArrayList<>(accelData.size());
                    tmpList.addAll(accelData.values());
                    Collections.sort(tmpList, Data.accelDataSorter);

                    //trim
                    if (tmpList.size() > accelDataPointsLimit) {
                        int delta = tmpList.size() - accelDataPointsLimit;
                        while (--delta > 0) {
                            AccelData oldData = tmpList.get(0);
                            tmpList.remove(oldData);
                            accelData.remove(oldData.ms);
                        }
                    }

                    //replace datasource for a View
                    accelArray.clear();
                    accelArray.addAll(tmpList);

                    //???????????????????? ?????????? ????????????
                    //long lastTimestamp = accelArray.get(accelArray.size()-1).ms;
                    //calibrationFragment.lastTimestamp = lastTimestamp;

                    if (calibrationFragment !=null && calibrationFragment.getModel()!=null) {
                        try {
                            List<AccelData> newData = new ArrayList<>(accelArray);
                            calibrationFragment.getModel().updateData(newData);
                        } catch (Exception ex) {
                            ex.printStackTrace();
                        }
                    }

                } finally {
                    packetsProcessing.set(false);
                }


            }
        }
    };
}
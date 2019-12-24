package ru.track_it.motohelper;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.os.Build;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class MarshMallowPermission {
    public static final List<String> requiredPermissions = new ArrayList<>();
    public static boolean inRequestRoutine = false;
    static{
        requiredPermissions.add(Manifest.permission.BLUETOOTH);
        requiredPermissions.add(Manifest.permission.BLUETOOTH_ADMIN);
        requiredPermissions.add(Manifest.permission.ACCESS_COARSE_LOCATION);
        requiredPermissions.add(Manifest.permission.ACCESS_FINE_LOCATION);
    }


    public static boolean checkPermission(Activity activity) {
        boolean result = true;
        for(String permission : requiredPermissions){
            int test = ContextCompat.checkSelfPermission(activity, permission);
            if (test == PackageManager.PERMISSION_GRANTED){
                continue;
            } else {
                requestPermission(activity, permission);
                result = false;
                break;
            }
        }
        return result;
    }




    private static void requestPermission(final Activity activity, final String permission){
        final int requestCode = requiredPermissions.indexOf(permission)+1;
        //
        if (ActivityCompat.shouldShowRequestPermissionRationale(activity, permission) ||
                Build.VERSION.SDK_INT >= Build.VERSION_CODES.M){
            new AlertDialog.Builder(activity)
                    .setTitle("Permission needed")
                    .setMessage("Permission required for this application. otherwise close.")
                    .setPositiveButton("ok", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            ActivityCompat.requestPermissions(activity,new String[]{permission},requestCode);
                        }
                    })
                    .setNegativeButton("cancel", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                        }
                    })
                    .setOnDismissListener(new DialogInterface.OnDismissListener() {
                        @Override
                        public void onDismiss(DialogInterface dialog) {
                            inRequestRoutine = false;
                        }
                    })
                    .create().show();
            inRequestRoutine = true;
        } else {
            ActivityCompat.requestPermissions(activity,new String[]{permission},requestCode);
        }
    }


}

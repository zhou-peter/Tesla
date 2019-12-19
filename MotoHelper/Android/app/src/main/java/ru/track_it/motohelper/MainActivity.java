package ru.track_it.motohelper;

import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.tabs.TabLayout;

import androidx.viewpager.widget.ViewPager;
import androidx.appcompat.app.AppCompatActivity;

import android.view.View;

import ru.track_it.motohelper.Packets.AbstractInPacket;
import ru.track_it.motohelper.Packets.PacketsListener;
import ru.track_it.motohelper.Packets.PacketsManager;
import ru.track_it.motohelper.Packets.Utils;
import ru.track_it.motohelper.ui.main.SectionsPagerAdapter;

public class MainActivity extends AppCompatActivity {

    CommunicationManagerBLE communicationManager = new CommunicationManagerBLE(this);
    PacketsManager packetsManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        SectionsPagerAdapter sectionsPagerAdapter = new SectionsPagerAdapter(this,
                getSupportFragmentManager());
        ViewPager viewPager = findViewById(R.id.view_pager);
        viewPager.setAdapter(sectionsPagerAdapter);
        TabLayout tabs = findViewById(R.id.tabs);
        tabs.setupWithViewPager(viewPager);
        FloatingActionButton fab = findViewById(R.id.fab);

        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });


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
            while (!packetsManager.receivedPackets.isEmpty()) {
                AbstractInPacket packet = packetsManager.receivedPackets.poll();
                switch (packet.Command) {
                    case 0x01:
                        break;
                    default:
                        packet.DefaultProcess();
                }
            }
        }
    };
}
package cn.gocoding.common.ui;

import android.os.Bundle;
import android.os.Environment;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import cn.gocoding.common.Bizlayer;

import cn.gocoding.common.BizlayerCallback;
import cn.gocoding.common.R;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
        Bizlayer.initBiz(this, "lua", Environment.getExternalStorageDirectory().getAbsolutePath().concat("/common"));

//        Bizlayer.call("server.connect", null, new BizlayerCallback() {
//            @Override
//            public void callback(String data) {
//                System.out.println(data);
//            }
//        });



        for (int i = 0; i < 1; i ++) {
            Bizlayer.call("native.get_error_code", null, new BizlayerCallback() {
                @Override
                public void callback(String data) {
                    System.out.println(count);
                    count ++;
                }
            });
        }
//        for (int i = 0; i < 500; i ++) {
//            Bizlayer.logw("log to android : " + i);
//        }
//        for (int i = 0; i < 500; i ++) {
//            Bizlayer.loge("log to android : " + i);
//        }
    }
    private int count = 0;
}

package com.yunfeng.autotestinterface;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;

import com.snail.mobilesdk.platform.MobileSDKUtil;

class MainActivity extends Activity {

    private EditText sample_text;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        sample_text = findViewById(R.id.sample_text);
        String ip = MobileSDKUtil.getIPAddress();
        Log.i("IPADDR", ip);
    }

    public void onButtonClick(View view) {
        JNILib.sendmsg(sample_text.getText().toString());
    }

    public void onServerMessageClick(View view) {
        JNILib.sendservermsg(sample_text.getText().toString());
    }

    public void onClientClick(View view) {
        JNILib.startClient();
    }

    public void onServerClick(View view) {
        JNILib.startServer();
    }

    static {
        System.loadLibrary("native-lib");
    }
}

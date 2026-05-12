package com.v0id.madamesh;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(new TextView(this));
        
        TextView tv = (TextView) getContentView();
        tv.setText(stringFromJNI());
        
        // Démarrage du moteur sur le port par défaut 8888
        startEngine(8888);
    }

    public native String stringFromJNI();
    public native boolean startEngine(int port);
    public native void stopEngine();
    public native void discoverNodes();
}

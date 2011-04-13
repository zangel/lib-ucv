package com.baldzarika;

import android.app.Activity;
import android.os.Bundle;
import com.baldzarika.ar.*;


public class BaldzAR extends Activity {
    
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
    }
    
    private Size2 m_FrameSize=new Size2(640,480);
    private Frame m_PreviewFrame;//=new Frame(new Size2(640,480));
}
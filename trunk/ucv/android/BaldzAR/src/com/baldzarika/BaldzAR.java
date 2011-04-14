package com.baldzarika;

import android.app.Activity;
import android.os.Bundle;

import com.baldzarika.ar.*;

public class BaldzAR extends Activity implements Tracker.MarkerStateCallback {
    /** Called when the activity is first created. */
	
	@Override
	public void onStateChanged(Tracker.MarkerState ms, int sc) {
		int ssc=sc;
	}
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        Marker testMarker=new Marker();
        testMarker.load("/sdcard/box.png");
        
        Size2 testMarkerSize=testMarker.getSize();
        Tracker testTracker=new Tracker(testMarkerSize);
        testTracker.m_MSCB=this;
        testTracker.addMarker(testMarker);
        testTracker.start();
        testTracker.stop();
        testTracker.dispose();
        testTracker=null;
        testMarker.dispose();
        testMarker=null;
    }
}
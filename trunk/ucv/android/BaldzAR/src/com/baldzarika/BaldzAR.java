package com.baldzarika;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.baldzarika.ar.*;

public class BaldzAR extends Activity implements Tracker.MarkerStateCallback {
    /** Called when the activity is first created. */
	
	@Override
	public void onStateChanged(Tracker.MarkerState ms, int sc) {
		if(ms==null)
			Log.i("BaldzAR", "onStateChanged(null," + Integer.toString(sc) + ")" );
		else
			Log.i("BaldzAR", "onStateChanged(" + ms.toString() + "," + Integer.toString(sc) + ")" );
	}
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
    	ClassLoader this_cl=this.getClass().getClassLoader();
    	ClassLoader thread_cl=Thread.currentThread().getClass().getClassLoader();
    	ClassLoader system_cl=ClassLoader.getSystemClassLoader();
    	ClassLoader size2_cl=new Size2().getClass().getClassLoader();
    	
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        Marker testMarker1=new Marker();
        Marker testMarker2=new Marker();
        testMarker1.load("/sdcard/box.png");
        testMarker2.load("/sdcard/box.png");
        
        Size2 testMarkerSize=testMarker1.getSize();
        Tracker testTracker=new Tracker(testMarkerSize);
        testTracker.m_MSCB=this;
        testTracker.addMarker(testMarker1);
        testTracker.addMarker(testMarker2);
        testTracker.start();
        testTracker.stop();
        testTracker.dispose();
        testTracker=null;
        testMarker1.dispose();
        testMarker1=null;
        testMarker2.dispose();
        testMarker2=null;
    }
}
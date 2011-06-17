package com.baldzarika.ar.markerless;

import com.baldzarika.ar.Size2;

public class Tracker extends com.baldzarika.ar.Tracker
{
	static { System.loadLibrary("com.baldzarika.ar"); }
	
	public static class MarkerState extends com.baldzarika.ar.Tracker.MarkerState
	{
		protected MarkerState(long px) { super(px); }
		public native Marker getMarker();
	}
	
	
	private static native long create(Size2 frameSize);
	
	public Tracker(Size2 frameSize){ super(create(frameSize)); }
	
	public native boolean addMarker(Marker marker);
	public native boolean removeMarker(Marker marker);
	
}

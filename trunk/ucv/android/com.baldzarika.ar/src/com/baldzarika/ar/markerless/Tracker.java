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
	
	public native float getDetectionTreshold();
	public native boolean setDetectionTreshold(float treshold);
	
	public native int getDetectionMinFeatures();
	public native boolean setDetectionMinFeatures(int minFeatures);
	
	public native int getTrackingMaxFeatures();
	public native boolean setTrackingMaxFeatures(int maxFeatures);
	
	public native int getTrackingHalfWinSize();
	public native boolean setTrackingHalfWinSize(int halfWinSize);
	
	public native int getTrackingNumLevels();
	public native boolean setTrackingNumLevels(int numLevels);
	
	public native int getTrackingMaxIterations();
	public native boolean setTrackingMaxIterations(int maxIters);
	
	public native float getDetectionMaxDiffNorm();
	public native boolean setDetectionMaxDiffNorm(float maxDiffNorm);
	
	public native float getTrackingMaxDiffNorm();
	public native boolean setTrackingMaxDiffNorm(float maxDiffNorm);
	
	public native boolean addMarker(Marker marker);
}

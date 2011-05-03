package com.baldzarika.ar;

public class Tracker
{
	static { System.loadLibrary("com.baldzarika.ar"); }
	
	public static class MarkerState
	{
		public static final int SC_DETECTION=0;
		public static final int SC_POSE=1;
		public static final int SC_DETECT_NOTIFY=2;
		
		
		private native void create(long px);
		private native void destroy();
		
		protected MarkerState(long px)
		{
			create(px);
		}
		
		public void dispose()
		{
			destroy();
		}
		
		protected void finalize()
		{
			destroy();
		}
		
		public native boolean isDetected();
		public native Marker getMarker();
		public native android.graphics.Matrix getHomography();
		public native Point2[] getMarkerCorners();
		public native int getFeatureMatchesSize();
		
		private long m_px;
	}
	
	public interface Callback
	{
		public void onMarkerStateChanged(MarkerState ms, int sc);
		public void onTrackerStart(Tracker t);
		public void onTrackerStop(Tracker t);
		public void onTrackerStats(Tracker t, int nff);
	}
	
	private native void create(Size2 fs);
	private native void destroy();
	
	public Tracker(Size2 fs) {
		create(fs);
	}
	
	public void dispose()
	{
		destroy();
	}
	
	protected void finalize()
	{
		destroy();
	}
	
	public native Size2 getFrameSize();
	public native boolean setFrameSize(Size2 fs);
	
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
	
	
	
		
	
	public native MarkerState addMarker(Marker m);
	
	public native boolean start();
	public native boolean isStarted();
	public native boolean isActive();
	public native boolean stop();
	
	public native boolean update(Frame f);
	
	public boolean setCallback(Callback cb)
	{
		if(isActive()) return false;
		m_cb=cb;
		return true;
	}
	
	private long m_px=0;
	private Callback m_cb=null;
}

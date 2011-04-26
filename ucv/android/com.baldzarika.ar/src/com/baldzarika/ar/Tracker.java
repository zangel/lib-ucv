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

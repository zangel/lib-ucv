package com.baldzarika.ar;

public class Tracker
{
	static { System.loadLibrary("com.baldzarika.ar"); }
	
	public static final int RS_STOPPED=0;
	public static final int RS_STARTING=1;
	public static final int RS_STARTED=2;
	public static final int RS_STOPPING=3;
	
	
	public static class MarkerState
	{
		public static final int SC_DETECTION=0;
		public static final int SC_POSE=1;
		public static final int SC_DETECT_NOTIFY=2;
		
		
		protected native void initialize(long px);
		protected native void destroy();
		
		protected MarkerState(long px){ initialize(px); }
		public void dispose(){ destroy(); }
		protected void finalize(){ destroy(); }
		
		public native Tracker getTracker();
		public native boolean isDetected();
		public native Size2 getMarkerSize();
		public native android.graphics.Matrix getHomography();
		public native boolean getCameraPose(float [] cameraPose);
				
		private long m_px;
	}
	
	public interface Callback
	{
		public void onRunningStateChanged(Tracker t, int rs);
		public void onMarkerStateChanged(MarkerState ms, int sc);
	}
	
	protected native void initialize(long px);
	protected native void destroy();
	
	protected Tracker(long px){ initialize(px); }
	
	public void dispose(){ destroy(); }
	protected void finalize(){ destroy(); }
	
	public native Size2 getFrameSize();
	public native boolean setFrameSize(Size2 fs);
	
	public native float getCameraFovy();
	public native boolean setCameraFovy(float fovy);
	
	public native float getCameraZNear();
	public native boolean setCameraZNear(float zNear);
	
	public native float getCameraZFar();
	public native boolean setCameraZFar(float zFar);
	
	public native float getCameraFocalLength();
	
	public native boolean getCameraProjection(float [] cameraProjection);
	
	public native boolean start();
	public native boolean isActive();
	public native boolean isStarting();
	public native boolean isStarted();
	public native boolean stop();
	public native boolean waitToStop();
	
	public native boolean processFrame(byte[] frame, int pfmt, int width, int height);
	
	//for testing purpose only ( it is very inefficient )
	public native boolean processFrame(android.graphics.Bitmap frame);
	
	
	public boolean setCallback(Callback cb)
	{
		if(isActive()) return false;
		m_cb=cb;
		return true;
	}
	
	private long m_px=0;
	private Callback m_cb=null;
}

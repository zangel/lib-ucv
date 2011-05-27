package com.baldzarika.ar.fiducial;

import com.baldzarika.ar.Size2;


public class Detector
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
		
		private native void create(long px);
		private native void destroy();
		
		public void dispose(){ destroy(); }
		
		protected void finalize(){ destroy(); }
		
		protected MarkerState(long px)
		{
			create(px);
		}
		
		public native boolean isDetected();
		public native int getMarkerId();
		public native MarkerModel getMarkerModel();
		public native Detector getDetector();
		public native android.graphics.Matrix getHomography();
		public native boolean getCameraPose(float [] cameraPose);
		
		private long m_px;
	}

	public interface Callback
	{
		public void onRunningStateChanged(Detector detector, int rs);
		public void onMarkerStateChanged(MarkerState markerState, int sc);
	}
	
	private native void create(Size2 frameSize);
	private native void destroy();
	
	public void dispose(){ destroy(); }
	protected void finalize(){ destroy(); }
	
	public Detector(Size2 frameSize)
	{
		create(frameSize);
	}
	
	public native boolean addMarkerModel(MarkerModel markerModel);
	public native boolean removeMarkerModel(MarkerModel markerModel);
	
	public native Size2 getFrameSize();
	public native boolean setFrameSize(Size2 frameSize);
	
	public native float getCameraFovy();
	public native boolean setCameraFovy(float fovy);
	
	public native float getCameraFocalLength();
	public native boolean getCameraProjection(float [] cameraProjection);
		
	
	public native boolean start();
	public native boolean isActive();
	public native boolean isStarting();
	public native boolean isStarted();
	public native boolean stop();
	public native boolean waitToStop();
	
	public native boolean update(byte[] data, int pfmt, int width, int height);
	
	public boolean setCallback(Callback cb)
	{
		if(isActive()) return false;
		m_cb=cb;
		return true;
	}
	
	private long 		m_px=0;
	private Callback	m_cb=null;
}

package com.baldzarika.ar;

public class Tracker {
	
	static {
		System.loadLibrary("BaldzarikaAR");
	}
	
	private native void create(Size2 fs);
	private native void destroy();
	
	Tracker(Size2 fs){
		create(fs);
	}
	
	public void dispose(){
		destroy();
	}
	
	protected void finalize() {
		destroy();
	}
		
	public class MarkerState {
		
		public static final int SC_DETECTION=0;
		public static final int SC_POSE=1;
		
		private native void destroy();
		
		protected MarkerState(){
		}
		
		public void dispose(){
			destroy();
		}
		
		protected void finalize() {
			destroy();
		}
		
		public native boolean isDetected();
		public native Marker getMarker();
		
		private long m_px=0;
	}
	
	public static interface MarkerStateCallback {
		
		void process(MarkerState ms, int sc);
		
		public long m_px=0;
	}
	
	public native Size2 getFrameSize();
	public native boolean setFrameSize(Size2 fs);
	
	public native MarkerState addMarker(Marker m);
	
	public native boolean start();
	public native boolean isStarted();
	public native boolean stop();
	
	public native boolean update(Frame frame);
	
	private long m_px=0;
}

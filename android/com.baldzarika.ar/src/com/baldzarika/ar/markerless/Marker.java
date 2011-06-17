package com.baldzarika.ar.markerless;

import com.baldzarika.ar.Size2;

public class Marker
{
	static { System.loadLibrary("com.baldzarika.ar"); }
	
	private native void initialize(long px);
	private static native long create();
	private static native long create(Size2 ms);
	private native void destroy();
		
	public Marker(){ initialize(create()); }
	public Marker(Size2 ms){ initialize(create(ms)); }
	
	public void dispose(){ destroy(); }
	protected void finalize(){ destroy(); }
	
	public native Size2 getSize();
	public native boolean loadImage(String fileName);
	public native boolean setImage(android.graphics.Bitmap bitmap);
	public native boolean saveImage(String fileName);
		
	private long m_px=0;
}

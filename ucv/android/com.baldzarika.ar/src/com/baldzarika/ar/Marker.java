package com.baldzarika.ar;

public class Marker
{
	static { System.loadLibrary("com.baldzarika.ar"); }
	
	private native void create();
	private native void create(Size2 ms);
	private native void destroy();
		
	public Marker()
	{
		create();
	}

	public Marker(Size2 ms)
	{
		create(ms);
	}
	
	public void dispose()
	{
		destroy();
	}
	
	protected void finalize()
	{
		destroy();
	}
	
	public native Size2 getSize();
	public native boolean loadImage(String fileName);
	public native boolean setImage(android.graphics.Bitmap bitmap);
	public native boolean saveImage(String fileName);
		
	private long m_px=0;
}

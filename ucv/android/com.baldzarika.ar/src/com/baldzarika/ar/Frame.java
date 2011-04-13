package com.baldzarika.ar;

public class Frame {
	
	static {
		System.loadLibrary("com.baldzarika.ar");
	}
	
	public static final int PFMT_NV16=16;
	public static final int PFMT_NV21=17;
	
	private native void create();
	private native void create(Size2 is);
	private native void destroy();
	
	public Frame(){
		create();
	}
	
	public Frame(Size2 is){
		create(is);
	}
	
	public void dispose(){
		destroy();
	}
	
	protected void finalize() {
		destroy();
	}
	
	public native Size2 getSize();
	public native double getMedianPixelValue();
	
	public native boolean setPixels(byte[] data, int pfmt);
	
	private long m_px=0;
}

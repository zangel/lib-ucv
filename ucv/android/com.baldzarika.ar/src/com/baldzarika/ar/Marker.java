package com.baldzarika.ar;

public class Marker {
	
	static {
		System.loadLibrary("BaldzarikaAR");
	}
	
	private native void create(Size2 ms);
	private native void create(Marker that);
	private native void destroy();

	public Marker(Size2 ms){
		create(ms);
	}
	
	public Marker(Marker that){
		create(that);
	}
	
	public void dispose(){
		destroy();
	}
	
	protected void finalize() {
		destroy();
	}
	
	public native Size2 getSize();
	public native boolean load(String fileName);
		
	private long m_px=0;
}

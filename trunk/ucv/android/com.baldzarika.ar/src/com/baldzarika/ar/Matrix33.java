package com.baldzarika.ar;

public class Matrix33
{
	static { System.loadLibrary("com.baldzarika.ar"); }
	
	private native void create();
	private native void destroy();
	
	private long m_px;
}

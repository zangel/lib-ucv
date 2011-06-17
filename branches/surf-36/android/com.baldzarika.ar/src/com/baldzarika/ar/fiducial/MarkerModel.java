package com.baldzarika.ar.fiducial;

public abstract class MarkerModel
{
	static { System.loadLibrary("com.baldzarika.ar"); }
	
	private native void initialize(long px);
	private native void destroy();
	
	MarkerModel(long px){ initialize(px); }
	
	public void dispose(){ destroy(); }
	protected void finalize(){ destroy(); }
	
	private long m_px=0;
}

package com.baldzarika.ar.fiducial;

public class BCHMarkerModel extends MarkerModel
{
	static { System.loadLibrary("com.baldzarika.ar"); }
	
	private native void create();
	
	public BCHMarkerModel()
	{
		create();
	}
}

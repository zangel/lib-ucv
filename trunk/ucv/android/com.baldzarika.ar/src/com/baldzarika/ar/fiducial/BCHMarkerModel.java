package com.baldzarika.ar.fiducial;

public class BCHMarkerModel extends MarkerModel
{
	static { System.loadLibrary("com.baldzarika.ar"); }
	
	private static native long create();
	
	public BCHMarkerModel()
	{
		super(create());
	}
}

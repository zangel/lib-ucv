package com.baldzarika;

import android.app.Application;
import android.content.res.Configuration;

import com.baldzarika.ar.Frame;
import com.baldzarika.ar.Marker;
import com.baldzarika.ar.Tracker;
import com.baldzarika.ar.Size2;


public class BaldzARApp extends Application
{
	@Override
	public final void onCreate()
	{
		super.onCreate();
		s_Instance=this;
	}
	
	@Override
	public final void onTerminate()
	{
		super.onTerminate();
	}
	
	@Override
	public final void onLowMemory()
	{
		super.onLowMemory();
	}
	
	@Override
	public final void onConfigurationChanged(Configuration newConfig)
	{
		super.onConfigurationChanged(newConfig);
	}
	
	public static BaldzARApp getInstance()
	{
		return s_Instance;
	}
	
	public Tracker getTracker()
	{
		return m_Tracker;
	}
	
	public Marker getMarker()
	{
		return m_Marker;
	}
	
	public Frame getFrame()
	{
		return m_Frame;
	}
	
	public Tracker.MarkerState getMarkerState()
	{
		return m_MarkerState;
	}
	
	private static BaldzARApp s_Instance;
	public static final Size2 DEFAULT_TRACKER_SIZE=new Size2(640,480);
	
	private Tracker m_Tracker=new Tracker(DEFAULT_TRACKER_SIZE);
	private Marker m_Marker=new Marker();
	private Tracker.MarkerState m_MarkerState=m_Tracker.addMarker(m_Marker);
	private Frame m_Frame=new Frame(DEFAULT_TRACKER_SIZE);
}

package com.baldzarika;

import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.List;

import android.app.Application;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.hardware.Camera;
import android.preference.PreferenceManager;

import com.baldzarika.ar.Frame;
import com.baldzarika.ar.Marker;
import com.baldzarika.ar.Tracker;
import com.baldzarika.ar.Size2;


public class BaldzARApp extends Application implements SharedPreferences.OnSharedPreferenceChangeListener
{
	@Override
	public final void onCreate()
	{
		super.onCreate();
		s_Instance=this;
		
		Camera camera=Camera.open();
		if(camera!=null)
		{
			List<Camera.Size> cameraPreviewSizes=camera.getParameters().getSupportedPreviewSizes();
			for(Camera.Size cs: cameraPreviewSizes)
			{
				m_FrameSizes.add( new Size2(cs.width, cs.height));
			}
			camera.release();
			camera=null;
		}
		else
		{
			m_FrameSizes.add(new Size2(0,0));
		}
		
		SharedPreferences sp=PreferenceManager.getDefaultSharedPreferences(this);
		sp.registerOnSharedPreferenceChangeListener(this);
		
		m_DefaultFrameSize=null;
		
		for(Size2 sz: m_FrameSizes)
		{
			if(sz.m_Width>=320 && sz.m_Height>=240)
			{
				m_DefaultFrameSize = new StringBuilder().
					append(sz.m_Width).
					append("x").
					append(sz.m_Height).
					toString();
				break;
			}
		}
		if(m_DefaultFrameSize==null)
			throw new InvalidParameterException();
				
		String strFrameSize=sp.getString(getString(R.string.settings_frame_size_key), m_DefaultFrameSize);
		
		
		
		m_Tracker=new Tracker(DEFAULT_TRACKER_SIZE);
		private Marker m_Marker=new Marker();
		private Tracker.MarkerState m_MarkerState=m_Tracker.addMarker(m_Marker);
		private Frame m_Frame=new Frame(DEFAULT_TRACKER_SIZE);
		
	}
	
	@Override
	public final void onTerminate()
	{
		super.onTerminate();
		SharedPreferences sp=PreferenceManager.getDefaultSharedPreferences(this);
		sp.unregisterOnSharedPreferenceChangeListener(this);
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
	
	@Override
	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key)
	{
		if(key.equals(getString(R.string.settings_frame_size_key)))
		{
			setFrameSize(getFrameSizeSettings(sharedPreferences));
		}
		else
		if(key.equals(getString(R.string.settings_detection_treshold_key)))
		{
		}
	}
	
	public static BaldzARApp getInstance()
	{
		return s_Instance;
	}
	
	public List<Size2> getFrameSizes()
	{
		return m_FrameSizes;		
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
	
	private void setFrameSize(Size2 frameSize)
	{
		m_Tracker.setFrameSize(frameSize);
		m_Frame.setSize(frameSize);
	}
	
	private Size2 getFrameSizeSettings(SharedPreferences sharedPreferences)
	{
		String frameSize=sharedPreferences.getString(getString(R.string.settings_frame_size_key), m_DefaultFrameSize);
		
		for(Size2 sz: m_FrameSizes)
		{
			if(frameSize.equals(new StringBuilder().
					append(sz.m_Width).
					append("x").
					append(sz.m_Height).
					toString())
			)
			{
				return sz;
			}
		}
		return new Size2(0,0);
	}
	
	private static BaldzARApp s_Instance;
	
	private List<Size2> m_FrameSizes=new ArrayList<Size2>();	
	private String m_DefaultFrameSize;
	private Tracker m_Tracker=new Tracker(DEFAULT_TRACKER_SIZE);
	private Marker m_Marker=new Marker();
	private Tracker.MarkerState m_MarkerState=m_Tracker.addMarker(m_Marker);
	private Frame m_Frame=new Frame(DEFAULT_TRACKER_SIZE);
}

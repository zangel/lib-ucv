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
		
		m_FrameSizes=new ArrayList<Size2>();
		
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
			throw new RuntimeException("Unable to open camera!");
		
		SharedPreferences sp=PreferenceManager.getDefaultSharedPreferences(this);
		sp.registerOnSharedPreferenceChangeListener(this);
		
		SharedPreferences.Editor spEdit=sp.edit();
		
		
		
		int closestFSIdx=0;
		Size2 closestFS=m_FrameSizes.get(closestFSIdx);
		double closeness=Math.pow(closestFS.m_Width-DEFAULT_FRAME_SIZE.m_Width, 2)+Math.pow(closestFS.m_Height-DEFAULT_FRAME_SIZE.m_Height, 2);
		for(int ifs=1;ifs<m_FrameSizes.size();++ifs)
		{
			Size2 candidate=m_FrameSizes.get(ifs);
			
			double candidateCloseness=Math.pow(candidate.m_Width-DEFAULT_FRAME_SIZE.m_Width, 2)+Math.pow(candidate.m_Height-DEFAULT_FRAME_SIZE.m_Height, 2);
			if(candidateCloseness<=closeness)
			{
				closestFS=candidate;
				closeness=candidateCloseness;
			}
		}
		
		m_DefaultFrameSize=new StringBuilder().
			append(closestFS.m_Width).
			append("x").
			append(closestFS.m_Height).
			toString();
		
		Size2 frameSize=getFrameSizeSettings(sp, spEdit);
		
		
		
		m_Tracker=new Tracker(frameSize);
		
		int detectionTreshold=sp.getInt(getString(R.string.settings_detection_treshold_key), DEFAULT_DETECTION_TRESHOLD);
		m_Tracker.setDetectionTreshold((float)Math.pow(10.0f, -4.0f+(detectionTreshold/50.0f)));
		if(!sp.contains(getString(R.string.settings_detection_treshold_key)))
			spEdit.putInt(getString(R.string.settings_detection_treshold_key), detectionTreshold);
		
		int minDetectionFeatures=sp.getInt(getString(R.string.settings_detection_min_features_key), DEFAULT_DETECTION_MIN_FEATURES);
		m_Tracker.setDetectionMinFeatures(minDetectionFeatures);
		if(!sp.contains(getString(R.string.settings_detection_min_features_key)))
			spEdit.putInt(getString(R.string.settings_detection_min_features_key), minDetectionFeatures);
		
		int maxTrackingFeatures=sp.getInt(getString(R.string.settings_tracking_max_features_key), DEFAULT_TRACKING_MAX_FEATURES);
		m_Tracker.setTrackingMaxFeatures(maxTrackingFeatures);
		if(!sp.contains(getString(R.string.settings_tracking_max_features_key)))
			spEdit.putInt(getString(R.string.settings_tracking_max_features_key), maxTrackingFeatures);
		
		int trackingHalfWinSize=sp.getInt(getString(R.string.settings_tracking_half_win_size_key), DEFAULT_TRACKING_HALF_WIN_SIZE);
		m_Tracker.setTrackingHalfWinSize(trackingHalfWinSize);
		if(!sp.contains(getString(R.string.settings_tracking_half_win_size_key)))
			spEdit.putInt(getString(R.string.settings_tracking_half_win_size_key), trackingHalfWinSize);
		
		int trackingNumLevels=sp.getInt(getString(R.string.settings_tracking_num_levels_key), DEFAULT_TRACKING_NUM_LEVELS);
		m_Tracker.setTrackingNumLevels(trackingNumLevels);
		if(!sp.contains(getString(R.string.settings_tracking_num_levels_key)))
			spEdit.putInt(getString(R.string.settings_tracking_num_levels_key), trackingNumLevels);
		
		int trackingMaxIterations=sp.getInt(getString(R.string.settings_tracking_max_iterations_key), DEFAULT_TRACKING_MAX_ITERATIONS);
		m_Tracker.setTrackingMaxIterations(trackingMaxIterations);
		if(!sp.contains(getString(R.string.settings_tracking_max_iterations_key)))
			spEdit.putInt(getString(R.string.settings_tracking_max_iterations_key), trackingMaxIterations);
		
		m_Marker=new Marker();
		m_MarkerState=m_Tracker.addMarker(m_Marker);
		m_Frame=new Frame(frameSize);
		
		spEdit.commit();
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
			setFrameSize(getFrameSizeSettings(sharedPreferences,null));
		}
		else
		if(key.equals(getString(R.string.settings_detection_treshold_key)))
		{
			int detectionTreshold=sharedPreferences.getInt(key, DEFAULT_DETECTION_TRESHOLD);
			m_Tracker.setDetectionTreshold((float)Math.pow(10.0f, -4.0f+(detectionTreshold/50.0f)));
		}
		else
		if(key.equals(getString(R.string.settings_detection_min_features_key)))
		{
			int detectionMinFeatures=sharedPreferences.getInt(key, DEFAULT_DETECTION_MIN_FEATURES);
			m_Tracker.setDetectionMinFeatures(detectionMinFeatures);
		}
		else
		if(key.equals(getString(R.string.settings_tracking_max_features_key)))
		{
			int trackingMaxFeatures=sharedPreferences.getInt(key, DEFAULT_TRACKING_MAX_FEATURES);
			m_Tracker.setTrackingMaxFeatures(trackingMaxFeatures);
		}
		else
		if(key.equals(getString(R.string.settings_tracking_half_win_size_key)))
		{
			int trackingHalfWinSize=sharedPreferences.getInt(key, DEFAULT_TRACKING_HALF_WIN_SIZE);
			m_Tracker.setTrackingHalfWinSize(trackingHalfWinSize);
		}
		else
		if(key.equals(getString(R.string.settings_tracking_num_levels_key)))
		{
			int trackingNumLevels=sharedPreferences.getInt(key, DEFAULT_TRACKING_NUM_LEVELS);
			m_Tracker.setTrackingNumLevels(trackingNumLevels);
		}
		else
		if(key.equals(getString(R.string.settings_tracking_max_iterations_key)))
		{
			int trackingMaxIterations=sharedPreferences.getInt(key, DEFAULT_TRACKING_MAX_ITERATIONS);
			m_Tracker.setTrackingMaxIterations(trackingMaxIterations);
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
		if(m_Tracker!=null)
		{
			m_Tracker.setFrameSize(frameSize);
			m_Frame.setSize(frameSize);
		}
	}
	
	private Size2 getFrameSizeSettings(SharedPreferences sharedPreferences, SharedPreferences.Editor spEditor)
	{
		String frameSize=sharedPreferences.getString(getString(R.string.settings_frame_size_key), m_DefaultFrameSize);
		if(spEditor!=null && !sharedPreferences.contains(getString(R.string.settings_frame_size_key)))
			spEditor.putString(getString(R.string.settings_frame_size_key), frameSize);
		
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
	
	private static BaldzARApp s_Instance=null;
	
	private List<Size2> m_FrameSizes=null;	
	private String m_DefaultFrameSize=null;
	private Tracker m_Tracker=null;
	private Marker m_Marker=null;
	private Tracker.MarkerState m_MarkerState=null;
	private Frame m_Frame=null;
	
	public static final Size2 DEFAULT_FRAME_SIZE=new Size2(320,240);
	
	public static final int MIN_DETECTION_TRESHOLD=0;
	public static final int MAX_DETECTION_TRESHOLD=100;
	public static final int DEFAULT_DETECTION_TRESHOLD=30;
	
	public static final int MIN_DETECTION_MIN_FEATURES=8;
	public static final int MAX_DETECTION_MIN_FEATURES=16;
	public static final int DEFAULT_DETECTION_MIN_FEATURES=8;
	
	public static final int MIN_TRACKING_MAX_FEATURES=8;
	public static final int MAX_TRACKING_MAX_FEATURES=16;
	public static final int DEFAULT_TRACKING_MAX_FEATURES=16;
	
	public static final int MIN_TRACKING_HALF_WIN_SIZE=2;
	public static final int MAX_TRACKING_HALF_WIN_SIZE=8;
	public static final int DEFAULT_TRACKING_HALF_WIN_SIZE=3;
	
	public static final int MIN_TRACKING_NUM_LEVELS=1;
	public static final int MAX_TRACKING_NUM_LEVELS=4;
	public static final int DEFAULT_TRACKING_NUM_LEVELS=4;
	
	public static final int MIN_TRACKING_MAX_ITERATIONS=1;
	public static final int MAX_TRACKING_MAX_ITERATIONS=100;
	public static final int DEFAULT_TRACKING_MAX_ITERATIONS=2;
}

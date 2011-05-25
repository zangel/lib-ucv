package com.itgma;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.baldzarika.ar.Size2;
import com.baldzarika.ar.fiducial.BCHMarkerModel;
import com.baldzarika.ar.fiducial.Detector;

import android.app.Application;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.hardware.Camera;
import android.preference.PreferenceManager;

public class DemoApp extends Application implements SharedPreferences.OnSharedPreferenceChangeListener
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
		
		m_Detector=new Detector(frameSize);
		m_BCHMarkerModel = new BCHMarkerModel();
		m_Detector.addMarkerModel(m_BCHMarkerModel);
		
		
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
	}
	
	public static DemoApp getInstance()
	{
		return s_Instance;
	}
	
	public Detector getDetector()
	{
		return m_Detector;
	}

	public List<Size2> getFrameSizes()
	{
		return m_FrameSizes;		
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
	
	private void setFrameSize(Size2 frameSize)
	{
		if(m_Detector!=null)
		{
			m_Detector.setFrameSize(frameSize);
		}
	}
	
	private static DemoApp s_Instance=null;
	public static final Size2 DEFAULT_FRAME_SIZE=new Size2(320,240);
	
	private List<Size2> m_FrameSizes=null;
	private String m_DefaultFrameSize=null;
	private Detector m_Detector=null;
	private BCHMarkerModel m_BCHMarkerModel=null;
}

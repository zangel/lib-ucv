package com.itgma;

import java.util.ArrayList;
import java.util.List;

import com.baldzarika.ar.Size2;
import com.baldzarika.ar.fiducial.BCHMarkerModel;
import com.baldzarika.ar.fiducial.Detector;

import android.app.Application;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.hardware.Camera;
import android.preference.PreferenceManager;
import android.util.DisplayMetrics;
import android.view.WindowManager;

public class DemoApp extends Application implements SharedPreferences.OnSharedPreferenceChangeListener
{
	@Override
	public final void onCreate()
	{
		super.onCreate();
		s_Instance=this;
		
		m_PreviewSizes=new ArrayList<Size2>();
		
		Camera camera=Camera.open();
		if(camera!=null)
		{
			List<Camera.Size> cameraPreviewSizes=camera.getParameters().getSupportedPreviewSizes();
			for(Camera.Size cs: cameraPreviewSizes)
			{
				m_PreviewSizes.add( new Size2(cs.width, cs.height));
			}
			camera.release();
			camera=null;
		}
		else
			throw new RuntimeException("Unable to open camera!");
		
		SharedPreferences sp=PreferenceManager.getDefaultSharedPreferences(this);
		sp.registerOnSharedPreferenceChangeListener(this);
		
		SharedPreferences.Editor spEdit=sp.edit();
		
		
		DisplayMetrics metrics = new DisplayMetrics();
		WindowManager wmgr=(WindowManager)getSystemService(WINDOW_SERVICE);
		wmgr.getDefaultDisplay().getMetrics(metrics);
		 
		Size2 screenSize=new Size2(metrics.widthPixels,metrics.heightPixels);
		
		
		int closestFSIdx=0;
		Size2 closestFS=m_PreviewSizes.get(closestFSIdx);
		double closeness=Math.pow(closestFS.m_Width-screenSize.m_Width, 2)+Math.pow(closestFS.m_Height-screenSize.m_Height, 2);
		for(int ifs=1;ifs<m_PreviewSizes.size();++ifs)
		{
			Size2 candidate=m_PreviewSizes.get(ifs);
			
			double candidateCloseness=Math.pow(candidate.m_Width-screenSize.m_Width, 2)+Math.pow(candidate.m_Height-screenSize.m_Height, 2);
			if(candidateCloseness<=closeness)
			{
				closestFS=candidate;
				closeness=candidateCloseness;
			}
		}
		
		m_DefaultPreviewSize=new StringBuilder().
			append(closestFS.m_Width).
			append("x").
			append(closestFS.m_Height).
			toString();
		
		m_PreviewSize=getPreviewSizeSettings(sp, spEdit);
		
		m_Detector=new Detector(findOptimalFrameSize(m_PreviewSize));
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
		if(key.equals(getString(R.string.settings_preview_size_key)))
		{
			setPreviewSize(getPreviewSizeSettings(sharedPreferences,null));
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

	public List<Size2> getPreviewSizes()
	{
		return m_PreviewSizes;		
	}
	
	public Size2 getPreviewSize()
	{
		return m_PreviewSize;
	}
	
	private Size2 findOptimalFrameSize(Size2 previewSize)
	{
		if(previewSize.m_Width*m_PreviewSize.m_Height > 2*OPTIMAL_FRAME_SIZE.m_Width*OPTIMAL_FRAME_SIZE.m_Height)
		{
			return new Size2(previewSize.m_Width/2, previewSize.m_Height/2);
		}
		return previewSize;
	}
	
	private Size2 getPreviewSizeSettings(SharedPreferences sharedPreferences, SharedPreferences.Editor spEditor)
	{
		String previewSize=sharedPreferences.getString(getString(R.string.settings_preview_size_key), m_DefaultPreviewSize);
		if(spEditor!=null && !sharedPreferences.contains(getString(R.string.settings_preview_size_key)))
			spEditor.putString(getString(R.string.settings_preview_size_key), previewSize);
		
		for(Size2 sz: m_PreviewSizes)
		{
			if(previewSize.equals(new StringBuilder().
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
	
	private void setPreviewSize(Size2 previewSize)
	{
		m_PreviewSize=previewSize;
		m_Detector.setFrameSize(findOptimalFrameSize(m_PreviewSize));
	}
	
	private static DemoApp s_Instance=null;
	public static final Size2 OPTIMAL_FRAME_SIZE=new Size2(320,240);
	
	private List<Size2> m_PreviewSizes=null;
	private String m_DefaultPreviewSize=null;
	private Size2 m_PreviewSize=null;
	private Detector m_Detector=null;
	private BCHMarkerModel m_BCHMarkerModel=null;
}

package com.baldzarika;

import java.util.ArrayList;
import java.util.List;

import com.baldzarika.ar.Size2;

import android.app.Application;
import android.content.SharedPreferences;
import android.hardware.Camera;
import android.preference.PreferenceManager;

public class BaldzARApp extends Application
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
	}
	
	public static BaldzARApp getInstance()
	{
		return s_Instance;
	}
	
	SharedPreferences getPreferences()
	{
		return PreferenceManager.getDefaultSharedPreferences(this);
	}
	
	public List<Size2> getPreviewSizes()
	{
		return m_PreviewSizes;		
	}
	
	private static BaldzARApp s_Instance=null;
	private List<Size2> m_PreviewSizes=null;	
}

package com.baldzarika;

import java.util.List;

import android.content.SharedPreferences;
import android.util.DisplayMetrics;
import android.view.WindowManager;

import com.baldzarika.ar.Size2;
import com.baldzarika.ar.Tracker;

public abstract class ARSystem
{
	public static String AR_FIDUCIAL_SYSTEM=new String("fiducial");
	public static String AR_MARKERLESS_SYSTEM=new String("markerless");
	
	public static final String DEFAULT_AR_MARKERLESS_SYSTEM=new String("fiducial");
	
	static 
	{
		SharedPreferences sp=BaldzARApp.getInstance().getPreferences();
		SharedPreferences.Editor spEdit=sp.edit();
		
		if(!sp.contains(BaldzARApp.getInstance().getString(R.string.settings_general_preview_size_key)))
			spEdit.putString(BaldzARApp.getInstance().getString(R.string.settings_general_preview_size_key),
				getPreviewSizeConfigString()
			);
		
		if(!sp.contains(BaldzARApp.getInstance().getString(R.string.settings_general_system_key)))
			spEdit.putString(BaldzARApp.getInstance().getString(R.string.settings_general_system_key),
				DEFAULT_AR_MARKERLESS_SYSTEM
			);
		spEdit.commit();
	}
	
	protected static String getPreviewSizeConfigString()
	{
		DisplayMetrics metrics = new DisplayMetrics();
		WindowManager wmgr=(WindowManager)BaldzARApp.getInstance().getSystemService(android.app.Application.WINDOW_SERVICE);
		wmgr.getDefaultDisplay().getMetrics(metrics);
		 
		Size2 screenSize=new Size2(metrics.widthPixels,metrics.heightPixels);
		
		List<Size2> previewSizes=BaldzARApp.getInstance().getPreviewSizes();
		
		
		int closestFSIdx=0;
		Size2 closestFS=previewSizes.get(closestFSIdx);
		double closeness=Math.pow(closestFS.m_Width-screenSize.m_Width, 2)+Math.pow(closestFS.m_Height-screenSize.m_Height, 2);
		for(int ifs=1;ifs<previewSizes.size();++ifs)
		{
			Size2 candidate=previewSizes.get(ifs);
			
			double candidateCloseness=Math.pow(candidate.m_Width-screenSize.m_Width, 2)+Math.pow(candidate.m_Height-screenSize.m_Height, 2);
			if(candidateCloseness<=closeness)
			{
				closestFS=candidate;
				closeness=candidateCloseness;
			}
		}
		
		return BaldzARApp.getInstance().getPreferences().getString(
			BaldzARApp.getInstance().getString(R.string.settings_general_preview_size_key),
			new StringBuilder().append(closestFS.m_Width).
			append("x").
			append(closestFS.m_Height).
			toString()
		);
		
		
	}
	
	public static Size2 getPreviewSizeConfig()
	{
		String [] pvWidthHeight=getPreviewSizeConfigString().split("x");
		
		return new Size2(
			Integer.parseInt(pvWidthHeight[0]),
			Integer.parseInt(pvWidthHeight[1])
		);
	}
	
	public static ARSystem create()
	{
		String arSystem=BaldzARApp.getInstance().getPreferences().getString(
			BaldzARApp.getInstance().getString(R.string.settings_general_system_key),
			AR_FIDUCIAL_SYSTEM
		);
		
		if(arSystem.compareTo(AR_FIDUCIAL_SYSTEM)==0)
			return new ARFiducialSystem(getPreviewSizeConfig());
		//else
		//if(arSystem.compareTo(AR_MARKERLESS_SYSTEM)==0)
		//	return new ARMarkerlessSystem(getPreviewSizeConfig());
		return null;
	}
	
	public abstract Tracker getTracker();
	public abstract boolean isCorrect(Tracker.MarkerState ms);
}

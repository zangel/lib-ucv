package com.baldzarika;

import java.io.FileNotFoundException;

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;


import android.net.Uri;
import com.baldzarika.ar.Size2;
import com.baldzarika.ar.Tracker;
import com.baldzarika.ar.Tracker.MarkerState;

public class ARMarkerlessSystem extends ARSystem
{
	protected static Uri getMarkerImageUri()
	{
		return Uri.parse(BaldzARApp.getInstance().getPreferences().getString(
			BaldzARApp.getInstance().getString(R.string.settings_markerless_marker_key),
			null
		));
	}
	
	public ARMarkerlessSystem(Size2 previewSize)
	{
		m_Tracker=new com.baldzarika.ar.markerless.Tracker(previewSize);
		m_Tracker.setCameraZNear(0.1f);
		m_Tracker.setCameraZFar(2500.0f);
		
		Uri markerImageUri=getMarkerImageUri();
		if(markerImageUri!=null)
		{
			BitmapFactory.Options options=new BitmapFactory.Options();
	    	options.inScaled=true;
	    	options.inPreferredConfig=Bitmap.Config.ARGB_8888;
	    	try
	    	{
				Bitmap markerBitmap=BitmapFactory.decodeStream(
					BaldzARApp.getInstance().getContentResolver().openInputStream(markerImageUri),
					null,
					options
				);
				
				if(markerBitmap!=null)
				{
					m_Marker=new com.baldzarika.ar.markerless.Marker();
					m_Marker.setImage(markerBitmap);
				}	
			}
	    	catch (FileNotFoundException e)
	    	{
				e.printStackTrace();
			}
		}
		
		if(m_Marker!=null)
			m_Tracker.addMarker(m_Marker);
	}
	
	@Override
	public Tracker getTracker()
	{
		return m_Tracker;
	}

	@Override
	public boolean isCorrect(MarkerState ms)
	{
		if(ms==null)
			return false;
		
		if(m_Marker==null)
			return false;
		
		if(ms.getClass().equals(com.baldzarika.ar.markerless.Tracker.MarkerState.class))
			return ((com.baldzarika.ar.markerless.Tracker.MarkerState)ms).getMarker()==m_Marker;
		
		return false;
	}
	
	private com.baldzarika.ar.markerless.Tracker m_Tracker=null;
	private com.baldzarika.ar.markerless.Marker m_Marker=null;
}

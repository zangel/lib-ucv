package com.baldzarika;

import android.content.SharedPreferences;

import com.baldzarika.ar.Size2;
import com.baldzarika.ar.Tracker;
import com.baldzarika.ar.Tracker.MarkerState;

public class ARFiducialSystem extends ARSystem
{
	protected static final String DEFAULT_MARKER_ID=new String("618"); 
	
	static 
	{
		SharedPreferences sp=BaldzARApp.getInstance().getPreferences();
		SharedPreferences.Editor spEdit=sp.edit();
		
		if(!sp.contains(BaldzARApp.getInstance().getString(R.string.settings_fiducial_marker_id_key)))
			spEdit.putString(BaldzARApp.getInstance().getString(R.string.settings_fiducial_marker_id_key),
				DEFAULT_MARKER_ID
			);
		spEdit.commit();
	}
	
	protected static int getMarkerIdConfig()
	{
		return Integer.valueOf(BaldzARApp.getInstance().getPreferences().getString(
			BaldzARApp.getInstance().getString(R.string.settings_fiducial_marker_id_key),
			DEFAULT_MARKER_ID
		));
	}
	
	public ARFiducialSystem(Size2 previewSize)
	{
		m_Tracker=new com.baldzarika.ar.fiducial.Tracker(previewSize);
		m_BCHMarkerModel=new com.baldzarika.ar.fiducial.BCHMarkerModel();
		m_Tracker.addMarkerModel(m_BCHMarkerModel);
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
		
		if(m_CorrectMarkerId==-1)
			return false;
		
		if(ms.getClass().equals(com.baldzarika.ar.fiducial.Tracker.MarkerState.class))
			return ((com.baldzarika.ar.fiducial.Tracker.MarkerState)ms).getMarkerId()==m_CorrectMarkerId;
		
		return false;
	}
	
	private com.baldzarika.ar.fiducial.Tracker m_Tracker=null;
	private com.baldzarika.ar.fiducial.BCHMarkerModel m_BCHMarkerModel=null;
	private int m_CorrectMarkerId=getMarkerIdConfig();
}

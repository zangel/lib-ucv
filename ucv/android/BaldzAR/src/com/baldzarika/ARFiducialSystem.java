package com.baldzarika;

import com.baldzarika.ar.Size2;
import com.baldzarika.ar.Tracker;
import com.baldzarika.ar.Tracker.MarkerState;

public class ARFiducialSystem extends ARSystem
{
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
	private int m_CorrectMarkerId=-1;
}

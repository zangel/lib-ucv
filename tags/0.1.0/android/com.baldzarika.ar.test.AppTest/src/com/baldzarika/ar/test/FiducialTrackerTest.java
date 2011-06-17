package com.baldzarika.ar.test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.baldzarika.ar.Size2;
import com.baldzarika.ar.Tracker.MarkerState;
import com.baldzarika.ar.fiducial.BCHMarkerModel;
import com.baldzarika.ar.fiducial.Tracker;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class FiducialTrackerTest extends android.test.AndroidTestCase
{
	private Bitmap m_Frame=null;
	protected void setUp() throws IOException
	{
		BitmapFactory.Options opts=new BitmapFactory.Options();
		opts.inPreferredConfig = Bitmap.Config.ARGB_8888;
		m_Frame=BitmapFactory.decodeStream(getContext().getAssets().open("fiducial/frame.png"), null, opts);
	}
	
	public void testCreateDestroy()
	{
		Tracker tracker=new Tracker(new Size2(100,100));
		tracker.dispose();
	}
	
	public void testStartStop() throws InterruptedException
	{
		Tracker tracker=new Tracker(new Size2(100,100));
		assertFalse(tracker.isActive());
		assertFalse(tracker.isStarted());
		assertFalse(tracker.isStarting());
		assertTrue(tracker.start());
		assertTrue(tracker.isActive());
		while(tracker.isStarting()) Thread.sleep(10);
		assertTrue(tracker.isStarted());
		assertTrue(tracker.stop());
		assertTrue(tracker.waitToStop());
		assertFalse(tracker.isActive());
		tracker.dispose();
	}
	
	public void testAddRemoveMarkerModel() throws InterruptedException
	{
		Tracker tracker=new Tracker(new Size2(100,100));
		BCHMarkerModel binaryCodedMarkerModel=new BCHMarkerModel();
		assertTrue(tracker.addMarkerModel(binaryCodedMarkerModel));
		assertTrue(tracker.start());
		while(!tracker.isStarted()) Thread.sleep(10);
		assertFalse(tracker.removeMarkerModel(binaryCodedMarkerModel));
		assertTrue(tracker.stop());
		assertTrue(tracker.waitToStop());
		assertFalse(tracker.isActive());
		assertTrue(tracker.removeMarkerModel(binaryCodedMarkerModel));
	}
	
	public void testProcessFrame() throws InterruptedException
	{
		Tracker tracker=new Tracker(new Size2(m_Frame.getWidth(),m_Frame.getHeight()));
		BCHMarkerModel binaryCodedMarkerModel=new BCHMarkerModel();
		assertTrue(tracker.addMarkerModel(binaryCodedMarkerModel));
		assertFalse(tracker.processFrame(m_Frame));
		assertTrue(tracker.start());
		while(!tracker.isStarted()) Thread.sleep(10);
		assertTrue(tracker.processFrame(m_Frame));
		assertTrue(tracker.stop());
		assertTrue(tracker.waitToStop());
		assertFalse(tracker.isActive());
		assertTrue(tracker.removeMarkerModel(binaryCodedMarkerModel));
	}
	
	public void testDetection() throws InterruptedException
	{
		Tracker tracker=new Tracker(new Size2(m_Frame.getWidth(),m_Frame.getHeight()));
		BCHMarkerModel binaryCodedMarkerModel=new BCHMarkerModel();
		assertTrue(tracker.addMarkerModel(binaryCodedMarkerModel));
		final List<Integer> detectedMarkerIds=new ArrayList<Integer>();
		tracker.setCallback( new com.baldzarika.ar.Tracker.Callback()
			{
				@Override
				public void onRunningStateChanged(com.baldzarika.ar.Tracker t, int rs)
				{
				}
				
				@Override
				public void onMarkerStateChanged(MarkerState ms, int sc)
				{
					if(sc==com.baldzarika.ar.Tracker.MarkerState.SC_DETECTION && ms!=null && ms.isDetected())
					{
						Tracker.MarkerState fiducialMarkerState=(Tracker.MarkerState)ms;
						detectedMarkerIds.add(new Integer(fiducialMarkerState.getMarkerId()));
					}
				}
			}
		);
		assertFalse(tracker.processFrame(m_Frame));
		assertTrue(tracker.start());
		while(!tracker.isStarted()) Thread.sleep(10);
		assertTrue(tracker.processFrame(m_Frame));
		assertTrue(tracker.stop());
		assertTrue(tracker.waitToStop());
		assertFalse(tracker.isActive());
		assertTrue(tracker.removeMarkerModel(binaryCodedMarkerModel));
		assertEquals(detectedMarkerIds.size(), 1);
		assertEquals(618, detectedMarkerIds.get(0).intValue());
	}
	
}

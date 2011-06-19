package com.baldzarika.ar.test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.baldzarika.ar.Size2;
import com.baldzarika.ar.Tracker.MarkerState;
import com.baldzarika.ar.markerless.Tracker;
import com.baldzarika.ar.markerless.Marker;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class MarkerlessTrackerTest extends android.test.AndroidTestCase
{
	private Bitmap m_Frame=null;
	private Bitmap m_Marker=null;
	
	protected void setUp() throws IOException
	{
		BitmapFactory.Options opts=new BitmapFactory.Options();
		opts.inPreferredConfig = Bitmap.Config.ARGB_8888;
		m_Frame=BitmapFactory.decodeStream(getContext().getAssets().open("markerless/frame.png"), null, opts);
		m_Marker=BitmapFactory.decodeStream(getContext().getAssets().open("markerless/marker.png"), null, opts);
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
	
	public void testAddMarker() throws InterruptedException
	{
		Tracker tracker=new Tracker(new Size2(100,100));
		Marker bentenMarker=new Marker();
		bentenMarker.setImage(m_Marker);
		assertTrue(tracker.addMarker(bentenMarker));
		assertTrue(tracker.start());
		while(!tracker.isStarted()) Thread.sleep(10);
		assertTrue(tracker.stop());
		assertTrue(tracker.waitToStop());
		assertFalse(tracker.isActive());
	}
	
	public void testProcessFrame() throws InterruptedException
	{
		Tracker tracker=new Tracker(new Size2(m_Frame.getWidth(),m_Frame.getHeight()));
		Marker bentenMarker=new Marker();
		bentenMarker.setImage(m_Marker);
		assertTrue(tracker.addMarker(bentenMarker));
		assertFalse(tracker.processFrame(m_Frame));
		assertTrue(tracker.start());
		while(!tracker.isStarted()) Thread.sleep(10);
		assertTrue(tracker.processFrame(m_Frame));
		assertTrue(tracker.stop());
		assertTrue(tracker.waitToStop());
		assertFalse(tracker.isActive());
	}
	
	public void testDetection() throws InterruptedException
	{
		Tracker tracker=new Tracker(new Size2(m_Frame.getWidth(),m_Frame.getHeight()));
		Marker bentenMarker=new Marker();
		bentenMarker.setImage(m_Marker);
		assertTrue(tracker.addMarker(bentenMarker));
		
		final List<Marker> detectedMarkers=new ArrayList<Marker>();
		
		tracker.setCallback( new com.baldzarika.ar.Tracker.Callback()
			{
				public void onRunningStateChanged(com.baldzarika.ar.Tracker t, int rs)
				{
				}
				
				public void onMarkerStateChanged(MarkerState ms, int sc)
				{
					if(sc==com.baldzarika.ar.Tracker.MarkerState.SC_DETECTION && ms!=null && ms.isDetected())
					{
						Tracker.MarkerState markerlessMarkerState=(Tracker.MarkerState)ms;
						detectedMarkers.add(markerlessMarkerState.getMarker());
					}
				}
			}
		);
		
		assertFalse(tracker.processFrame(m_Frame));
		assertTrue(tracker.start());
		while(!tracker.isStarted()) Thread.sleep(10);
		assertTrue(tracker.processFrame(m_Frame));
		assertTrue(tracker.processFrame(m_Frame));
		assertTrue(tracker.stop());
		assertTrue(tracker.waitToStop());
		assertFalse(tracker.isActive());
		assertEquals(detectedMarkers.size(), 1);
		assertEquals(bentenMarker, detectedMarkers.get(0));
	}
}

package com.baldzarika.ar.test;

import com.baldzarika.ar.Size2;
import com.baldzarika.ar.fiducial.Tracker;

import junit.framework.TestCase;

public class FiducialTest extends  TestCase
{
	
	protected void setUp()
	{
	}
	
	public void testTrackerCreateDestroy()
	{
		Tracker tracker=new Tracker(new Size2(100,100));
		tracker.dispose();
	}
	
	public void testTrackerStartStop() throws InterruptedException
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
	
	
}

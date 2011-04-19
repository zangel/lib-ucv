package com.baldzarika;

import java.io.FileNotFoundException;
import java.io.IOException;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.app.Activity;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.os.Bundle;
import com.baldzarika.ar.*;


public class BaldzAR extends Activity
	implements Tracker.Callback, GLSurfaceView.Renderer,
		SurfaceHolder.Callback, Camera.PreviewCallback {
	
	public static final int 	SET_MARKER=0;
	public static final Size2 	TRACKER_SIZE=new Size2(640,480);
	
	//com.baldzarika.ar.Tracker.Callback
	@Override
	public void onMarkerStateChanged(Tracker.MarkerState ms, int sc) {
		Log.i("BaldzAR", "onStateChanged(" + ms.toString() + ", "+ Integer.toString(sc) + ")");
		
		if(ms!=null) {
			m_CameraPreview.performHapticFeedback(
					ms.isDetected()?HapticFeedbackConstants.KEYBOARD_TAP:HapticFeedbackConstants.LONG_PRESS,
				HapticFeedbackConstants.FLAG_IGNORE_VIEW_SETTING|HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING
			);
		}
	}
	
	@Override
	public void onTrackerStart(Tracker t) {
		updateMenu();
	}
	
	@Override
	public void onTrackerStop(Tracker t) {
		updateMenu();
	}
	
	@Override
	public void onTrackerStats(Tracker t, int nff) {
		Log.i("BaldzAR", "onTrackerStats(" + t.toString() + ", "+ Integer.toString(nff) + ")");
	}
		
	protected void updateMenu() {
		
		if(m_SetMarkerMI!=null)
			m_SetMarkerMI.setEnabled(
				m_Camera!=null &&
				m_Tracker!=null && !m_Tracker.isStarted()
			);
		
		if(m_StartTrackerMI!=null)
			m_StartTrackerMI.setEnabled(
				m_Camera!=null && 
				m_Tracker!=null && !m_Tracker.isStarted() &&
				m_Marker!=null && m_Marker.getSize().m_Width*m_Marker.getSize().m_Height!=0
			);
		
		if(m_StopTrackerMI!=null)
			m_StopTrackerMI.setEnabled(
				m_Camera!=null &&
				m_Tracker!=null && m_Tracker.isStarted()
			);
	}
	
	//Activity
	@Override
    public void onCreate(Bundle savedInstanceState) {
    	
		super.onCreate(savedInstanceState);
		
		m_Marker=new Marker();
		m_Frame=new Frame(TRACKER_SIZE);
		m_Tracker=new Tracker(TRACKER_SIZE);
		m_Tracker.setCallback(this);
		m_MarkerState=m_Tracker.addMarker(m_Marker);
		
    	m_GLView=new GLSurfaceView(this);
    	//m_GLView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
    	m_GLView.setRenderer(this);
    	m_GLView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    	m_GLView.getHolder().setFormat(PixelFormat.TRANSPARENT);
    	addContentView(m_GLView, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
    	
    	m_CameraPreview=new SurfaceView(this);
    	m_CameraPreview.getHolder().setFormat(PixelFormat.TRANSLUCENT);
    	m_CameraPreview.getHolder().setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    	m_CameraPreview.getHolder().addCallback(this);
    	addContentView(m_CameraPreview, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ViewGroup.LayoutParams.FILL_PARENT));
    }
	
	@Override
	public void onDestroy()
	{
		super.onDestroy();
		m_Tracker.stop();
		//m_Tracker.removeMarker(m_MarkerState);
		m_MarkerState.dispose();
		m_MarkerState=null;
		m_Tracker.dispose();
		m_Tracker=null;
		m_Frame.dispose();
		m_Frame=null;
		m_Marker.dispose();
		m_Marker=null;
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
	    MenuInflater inflater = getMenuInflater();
	    inflater.inflate(R.menu.baldzar_menu, menu);
	    m_Menu=menu;
	    if(m_Menu!=null) {
	    	m_SetMarkerMI=m_Menu.findItem(R.id.set_marker);
	    	m_StartTrackerMI=m_Menu.findItem(R.id.start_tracker);
	    	m_StopTrackerMI=m_Menu.findItem(R.id.stop_tracker);
	    }
	    updateMenu();
	    return true;
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
	    switch(item.getItemId()) {
	    	case R.id.set_marker: {
	    		startActivityForResult(new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI), SET_MARKER);
	        }
	    	break;
	    	
	    	case R.id.start_tracker: {
	    		m_Tracker.start();
	    		updateMenu();
	    	}
	    	break;
	    	
	    	case R.id.stop_tracker: {
	    		m_Tracker.stop();
	    		updateMenu();
	    	}
	    	break;
	    	
	    	default:
	    		return super.onOptionsItemSelected(item);
	    }
	    return true;
	}
	
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		
		switch(requestCode) {
			case SET_MARKER: {
				if(resultCode==Activity.RESULT_OK) {
					try {
						BitmapFactory.Options bmfo=new BitmapFactory.Options();
						bmfo.inScaled=false;
						bmfo.inPreferredConfig=Bitmap.Config.ARGB_8888;
						Bitmap markerBitmap=BitmapFactory.decodeStream(
							getContentResolver().openInputStream(data.getData()),
							null,
							bmfo
						);
						m_Marker.setImage(markerBitmap);
					}
					catch (FileNotFoundException e) {
						e.printStackTrace();
					}
					updateMenu();
				}
			}
			break;
		}
	}
	
	
	//android.opengl.GLSurfaceView.Renderer
	@Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
    }
	
	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		gl.glViewport(0, 0, width, height);
	}
	
	@Override
	public void onDrawFrame(GL10 gl)
	{
		gl.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
	}
	
	//android.view.SurfaceHolder.Callback
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
	{
		if(m_Camera!=null)
		{
			Camera.Parameters cameraParams=m_Camera.getParameters();
			cameraParams.setPreviewSize(TRACKER_SIZE.m_Width,TRACKER_SIZE.m_Height);
			//int pfmt=cameraParams.getPreviewFormat();
			//parameters->setPreviewFormat(0x10);
			//parameters->setPreviewFormat(0x11);
			m_Camera.setParameters(cameraParams);
			m_Camera.startPreview();
		}
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder holder)
	{
		m_Camera=Camera.open();
		if(m_Camera!=null)
		{
			try {
				m_Camera.setPreviewDisplay(m_CameraPreview.getHolder());
				m_Camera.setPreviewCallback(this);
			}
			catch (IOException e) {
				e.printStackTrace();
				m_Camera.release();
				m_Camera=null;
			}
		}
		updateMenu();
	}
	
	@Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
		if(m_Camera!=null)
		{
			m_Camera.stopPreview();
			try {
				m_Camera.setPreviewDisplay(null);
			}
			catch (IOException e) {
				e.printStackTrace();
			}
			m_Camera.setPreviewCallback(null);
			m_Camera.release();
			m_Camera=null;
		}
		updateMenu();
	}
	
	//android.hardware.Camera.PreviewCallback
	public void onPreviewFrame(byte[] data, Camera camera)
	{
		if(m_Tracker.isStarted() && m_Frame.setPixels(data, camera.getParameters().getPreviewFormat())) {
			m_Tracker.update(m_Frame);
		}
	}
    
	private GLSurfaceView		m_GLView=null;
	private SurfaceView			m_CameraPreview=null;
	private Camera				m_Camera=null;
	
	private Menu				m_Menu=null;
	private MenuItem			m_SetMarkerMI=null;
	private MenuItem			m_StartTrackerMI=null;
	private MenuItem			m_StopTrackerMI=null;
	
	
	private Marker				m_Marker=null;
	private Frame				m_Frame=null;
	private Tracker				m_Tracker=null;
	private Tracker.MarkerState	m_MarkerState=null;
}
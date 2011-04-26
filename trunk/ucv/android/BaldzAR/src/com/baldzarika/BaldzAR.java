package com.baldzarika;

import java.io.FileNotFoundException;
import java.io.IOException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.baldzarika.ar.Frame;
import com.baldzarika.ar.Marker;
import com.baldzarika.ar.Size2;
import com.baldzarika.ar.Tracker;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Bundle;
import android.os.Handler;
import android.sax.RootElement;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ToggleButton;

public class BaldzAR extends Activity implements Callback, PreviewCallback, Renderer, Tracker.Callback
{
	protected static final int SET_MARKER=0;
	
	//Activity
	@Override
    public void onCreate(Bundle savedInstanceState)
	{
    	super.onCreate(savedInstanceState);
    	
		m_CameraPreview=new SurfaceView(this);
    	m_CameraPreview.getHolder().addCallback(this);
    	m_CameraPreview.getHolder().setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        m_CameraPreview.getHolder().setFormat(PixelFormat.TRANSPARENT);
        
        setContentView(
       		m_CameraPreview,
       		new ViewGroup.LayoutParams(
       			ViewGroup.LayoutParams.FILL_PARENT,
       			ViewGroup.LayoutParams.FILL_PARENT
       		)
       	);
		    	
    	m_GLView=new GLSurfaceView(this);
    	m_GLView.setDebugFlags(GLSurfaceView.DEBUG_CHECK_GL_ERROR);
    	m_GLView.setEGLConfigChooser(8,8,8,8,16,0);
    	m_GLView.setRenderer(this);
    	m_GLView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
    	m_GLView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    	
    	addContentView(
    		m_GLView,
        	new ViewGroup.LayoutParams(
        		ViewGroup.LayoutParams.FILL_PARENT,
        		ViewGroup.LayoutParams.FILL_PARENT
        	)
        );
    	
    	m_GLView.setZOrderMediaOverlay(true);
    	
    	
    	
        LayoutInflater li = LayoutInflater.from(this);
    	m_HUDView=li.inflate(R.layout.hud, null);
    	
    	addContentView(
    		m_HUDView,
    		new ViewGroup.LayoutParams(
    			ViewGroup.LayoutParams.FILL_PARENT,
    			ViewGroup.LayoutParams.FILL_PARENT
    		)
    	);
    	
        final Button markerButton = (Button) findViewById(R.id.set_marker);
        markerButton.setOnClickListener( new OnClickListener()
        	{ public void onClick(View v) { onSetMarker(); } }
        );
        
        final Button trackButton = (Button) findViewById(R.id.start_stop_tracking);
        trackButton.setOnClickListener( new OnClickListener()
    		{ public void onClick(View v) { onStartStopTracking(); } }
        );
        
        final LevelIndicator markerLevel=(LevelIndicator)findViewById(R.id.marker_level);
    	markerLevel.setRange(0.0f, 8.0f, 16.0f);
        
        BaldzARApp.getInstance().getTracker().setCallback(this);
        
        updateHUDButtons();
    }
	
	@Override
	public void onDestroy()
	{
		super.onDestroy();
		BaldzARApp.getInstance().getTracker().setCallback(null);
	}
	
	@Override
	protected void onPause()
	{
		super.onPause();
		m_GLView.onPause();
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
		m_GLView.onResume();
	}
	
	@Override
    public void onActivityResult(int requestCode, int resultCode, Intent data)
    {
    	super.onActivityResult(requestCode, resultCode, data);
    	switch(requestCode)
    	{
    		case SET_MARKER:
    			if(resultCode==Activity.RESULT_OK) setMarker(data.getData());
    			break;
    	}
    }
	
	
	//android.view.SurfaceHolder.Callback
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
    	startCameraPreview();
    	updateHUDButtons();
    }
    
    @Override
	public void surfaceCreated(SurfaceHolder holder)
	{
    	openCamera();
    	updateHUDButtons();
    }
    
    @Override
	public void surfaceDestroyed(SurfaceHolder holder)
	{
    	closeCamera();
    	updateHUDButtons();
    }
    
    //android.hardware.Camera.PreviewCallback
    public void onPreviewFrame(byte[] data, Camera camera)
    {
    	Tracker tracker=BaldzARApp.getInstance().getTracker();
    	if(tracker.isStarted())
    	{
    		Frame frame=BaldzARApp.getInstance().getFrame();
    		if(frame.setPixels(data, camera.getParameters().getPreviewFormat()))
    		{
    			tracker.update(frame);
    		}
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
    
  //com.baldzarika.ar.Tracker.Callback
    @Override
    public void onMarkerStateChanged(Tracker.MarkerState ms, int sc)
    {
    	m_Handler.post(m_HUDMarkerLevelUpdateTask);
    }
    
    @Override
    public void onTrackerStart(Tracker t)
    {
    	m_Handler.post(m_HUDButtonsUpdateTask);
    	m_Handler.post(m_HUDMarkerLevelUpdateTask);
    	
    }
    
    @Override
    public void onTrackerStop(Tracker t)
    {
    	m_Handler.post(m_HUDButtonsUpdateTask);
    	m_Handler.post(m_HUDMarkerLevelUpdateTask);
    }
    
    @Override
    public void onTrackerStats(Tracker t, int nff)
    {
    }
    
    
    protected void updateHUDButtons()
    {
    	Tracker tracker=BaldzARApp.getInstance().getTracker();
    	Marker marker=BaldzARApp.getInstance().getMarker();
    	Size2 markerSize=marker.getSize();
    	
    	Button markerButton=(Button)findViewById(R.id.set_marker);
    	markerButton.setEnabled(m_Camera!=null && !tracker.isStarted());
    	    	
    	Button startStopTrackingButton=(Button)findViewById(R.id.start_stop_tracking);
    	startStopTrackingButton.setEnabled(m_Camera!=null && markerSize.m_Width*markerSize.m_Height!=0);
    	startStopTrackingButton.setBackgroundResource(tracker.isStarted()?R.drawable.stop_button:R.drawable.start_button);
    }
    
    private void updateMarkerLevel()
    {
    	LevelIndicator markerLevel=(LevelIndicator)findViewById(R.id.marker_level);
    	markerLevel.setValue(BaldzARApp.getInstance().getMarkerState().getFeatureMatchesSize());
    }
    
    private class HUDButtonsUpdateTask implements Runnable
    {
    	@Override
    	public void run()
    	{
    		updateHUDButtons();
    	}
    };
    
    private class HUDMarkerLevelUpdateTask implements Runnable
    {
    	@Override
    	public void run()
    	{
    		updateMarkerLevel();
    	}
    };
    
    protected void setMarker(Uri imgFileName)
    {
    	BitmapFactory.Options bmfo=new BitmapFactory.Options();
    	bmfo.inScaled=false;
    	bmfo.inPreferredConfig=Bitmap.Config.ARGB_8888;
    	try
    	{
			Bitmap markerBitmap=BitmapFactory.decodeStream(
				getContentResolver().openInputStream(imgFileName),
				null,
				bmfo
			);
			BaldzARApp.getInstance().getMarker().setImage(markerBitmap);
		}
    	catch (FileNotFoundException e)
    	{
			e.printStackTrace();
		}
    	updateHUDButtons();
    }
    
    protected void onSetMarker()
	{
    	startActivityForResult(new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI), SET_MARKER);
    }
    
    protected void onStartStopTracking()
	{
    	Tracker tracker=BaldzARApp.getInstance().getTracker();
    	
    	boolean success=false;
    	
    	if(tracker.isStarted())
    	{
    		success=tracker.stop();
    	}
    	else
    	{
    		success=tracker.start();
    	}
    	updateHUDButtons();
    }
    
	protected boolean startCameraPreview()
	{
		if(m_Camera!=null)
    	{
    		Camera.Parameters cameraParams=m_Camera.getParameters();
    		Size2 previewSize=BaldzARApp.getInstance().getTracker().getFrameSize();
    		cameraParams.setPreviewSize(previewSize.m_Width,previewSize.m_Height);
    		m_Camera.setParameters(cameraParams);
    		m_Camera.startPreview();
    		return true;
    	}
		return false;
	}
	
	protected boolean openCamera()
	{
		m_Camera=Camera.open();
    	if(m_Camera!=null)
    	{
    		try
    		{
    			m_Camera.setPreviewDisplay(m_CameraPreview.getHolder());
    			m_Camera.setPreviewCallback(this);
    			return true;
    		}
    		catch (IOException e)
    		{
    			e.printStackTrace();
    			m_Camera.release();
    			m_Camera=null;
    		}
    	}
    	return false;
	}
	
	protected boolean closeCamera()
	{
		if(m_Camera!=null)
	    {
    		m_Camera.stopPreview();
    		try
    		{
    			m_Camera.setPreviewDisplay(null);
    		}
    		catch (IOException e)
    		{
    			e.printStackTrace();
    		}
    		m_Camera.setPreviewCallback(null);
    		m_Camera.release();
    		m_Camera=null;
    		return true;
    	}
		return false;
	}
	
	private Handler							m_Handler=new Handler();
	private HUDButtonsUpdateTask			m_HUDButtonsUpdateTask=new HUDButtonsUpdateTask();
	private HUDMarkerLevelUpdateTask		m_HUDMarkerLevelUpdateTask=new HUDMarkerLevelUpdateTask();
	
	private SurfaceView                     m_CameraPreview=null;
    private GLSurfaceView           		m_GLView=null;
	private View							m_HUDView=null;
    private Camera                          m_Camera=null;
}
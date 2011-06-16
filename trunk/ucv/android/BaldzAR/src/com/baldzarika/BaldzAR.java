package com.baldzarika;

import java.io.IOException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.baldzarika.ar.Size2;
import com.baldzarika.ar.Tracker;
import com.baldzarika.ar.Tracker.Callback;

import android.app.Activity;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.Button;

public class BaldzAR extends Activity implements SurfaceHolder.Callback, GLSurfaceView.Renderer, Camera.PreviewCallback, Callback
{
	protected static final int SETTINGS_CHANGED=1;
	
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
    	
    	m_TrackingButton=(Button)findViewById(R.id.start_stop_tracking);
    	m_TrackingButton.setOnClickListener(new OnClickListener()
    		{ public void onClick(View v) { onStartStopDetecting(); } }
        );
        
        m_SettingsButton=(Button)findViewById(R.id.settings);
        m_SettingsButton.setOnClickListener( new OnClickListener()
    		{ public void onClick(View v) { onSettings(); } }
        );
        
        m_ARSystem=ARSystem.create();
        m_ARSystem.getTracker().setCallback(this);
        
        updateHUDButtons();
	}
    
    @Override
	public void onDestroy()
	{
		super.onDestroy();
		m_ARSystem.getTracker().setCallback(null);
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
    		case SETTINGS_CHANGED:
    			if(resultCode==Activity.RESULT_OK) settingsChanged();
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
    	stopCameraPreview();
    	closeCamera();
    	updateHUDButtons();
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
	
	//android.hardware.Camera.PreviewCallback
    public void onPreviewFrame(byte[] data, Camera camera)
    {
    	if(m_ARSystem.getTracker().isStarted())
    	{
    		m_ARSystem.getTracker().processFrame(
    			data,
    			camera.getParameters().getPreviewFormat(),
    			camera.getParameters().getPreviewSize().width,
    			camera.getParameters().getPreviewSize().height
    		);
    	}
    }
    
	//com.baldzarika.ar.Tracker.Callback
    @Override
    public void onRunningStateChanged(com.baldzarika.ar.Tracker tracker, int rs)
    {
    	m_Handler.removeCallbacks(m_HUDButtonsUpdateTask);
    	m_Handler.post(m_HUDButtonsUpdateTask);    	    	
    }
    
    public void onMarkerStateChanged(com.baldzarika.ar.Tracker.MarkerState markerState, int sc)
    {
    	if(m_ARSystem.isCorrect(markerState))
    	{
    		markerState.getCameraPose(m_CameraPose);
    		m_IsDetected=markerState.isDetected();
    	}
    	else
    		m_IsDetected=false;
    }
    
    protected boolean startCameraPreview()
	{
		if(m_Camera!=null)
    	{
    		Camera.Parameters cameraParams=m_Camera.getParameters();
    		Size2 previewSize=m_ARSystem.getTracker().getFrameSize();
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
	
	protected boolean stopCameraPreview()
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
    		return true;
    	}
		return false;
	}
	
	protected boolean closeCamera()
	{
		if(m_Camera!=null)
	    {
    		m_Camera.release();
    		m_Camera=null;
    		return true;
    	}
		return false;
	}
	
	protected void onStartStopDetecting()
	{
    	Tracker tracker=m_ARSystem.getTracker();
    	m_IsDetected=false;
    	
    	if(tracker.isStarted())
    	{
    		tracker.stop();
    	}
    	else
    	{
    		tracker.start();
    	}
    	m_TrackingButton.setEnabled(false);
    	m_SettingsButton.setEnabled(false);
    }
	
	protected void onSettings()
	{
		Intent launchSettingsIntent=new Intent().setClass(this, BaldzARSettings.class);
		startActivityForResult(launchSettingsIntent, SETTINGS_CHANGED);
	}
	
	protected void settingsChanged()
	{
		m_ARSystem.getTracker().setCallback(null);
		m_ARSystem=ARSystem.create();
		m_ARSystem.getTracker().setCallback(this);
	}

	protected void updateHUDButtons()
    {
    	Tracker tracker=m_ARSystem.getTracker();
    	
    	m_TrackingButton.setEnabled(m_Camera!=null && (!tracker.isActive() || tracker.isStarted()));
    	m_TrackingButton.setBackgroundResource((tracker.isStarted() || tracker.isStarting())?R.drawable.stop_button:R.drawable.start_button);
    	m_SettingsButton.setEnabled(m_Camera!=null && !tracker.isActive());
    }
    
    private class HUDButtonsUpdateTask implements Runnable
    {
    	@Override
    	public void run()
    	{
    		updateHUDButtons();
    	}
    };
    
    private SurfaceView                     m_CameraPreview=null;
    private GLSurfaceView           		m_GLView=null;
	private View							m_HUDView=null;
    private Camera                          m_Camera=null;
    
    private Handler							m_Handler=new Handler();
	private HUDButtonsUpdateTask			m_HUDButtonsUpdateTask=new HUDButtonsUpdateTask();
	
    private Button							m_TrackingButton=null;
	private Button							m_SettingsButton=null;
	
	private boolean							m_IsDetected=false;
	private float[]							m_CameraPose=new float[16];
	
	ARSystem								m_ARSystem=null;
}
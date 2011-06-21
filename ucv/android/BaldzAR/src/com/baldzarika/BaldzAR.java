package com.baldzarika;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

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

public class BaldzAR extends Activity implements SurfaceHolder.Callback, GLSurfaceView.Renderer, Camera.PreviewCallback, Camera.AutoFocusCallback, Callback 
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
    		{ public void onClick(View v) { onStartStopTracking(); } }
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
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
    	startCameraPreview();
    	updateHUDButtons();
    }
    
    public void surfaceCreated(SurfaceHolder holder)
	{
    	openCamera();
    	updateHUDButtons();
    }
    
    public void surfaceDestroyed(SurfaceHolder holder)
	{
    	stopCameraPreview();
    	closeCamera();
    	updateHUDButtons();
    }
    
	//android.opengl.GLSurfaceView.Renderer
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
	}

	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		gl.glViewport(0, 0, width, height);
	}
    
    public void onDrawFrame(GL10 gl)
    {
    	gl.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
		
		Tracker tracker=m_ARSystem.getTracker();
		if(tracker.isStarted() && m_IsDetected)
		{
			float [] projMatrix=new float[16];
			tracker.getCameraProjection(projMatrix);
	    	gl.glMatrixMode(GL10.GL_PROJECTION);
	    	gl.glLoadMatrixf(projMatrix, 0);
	    	
			gl.glMatrixMode(GL10.GL_MODELVIEW);
			gl.glLoadMatrixf(m_CameraPose, 0);
			
			gl.glEnable(GL10.GL_BLEND);
			gl.glDisable(GL10.GL_TEXTURE_2D);
			gl.glDisable(GL10.GL_DEPTH_TEST);
			gl.glDisable(GL10.GL_NORMALIZE);
			gl.glEnable(GL10.GL_DEPTH_TEST);
			gl.glDisable(GL10.GL_CULL_FACE);
			gl.glDisable(GL10.GL_LIGHTING);
			gl.glShadeModel(GL10.GL_SMOOTH);
			gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
			gl.glDisableClientState(GL10.GL_COLOR_ARRAY);
			gl.glDisableClientState(GL10.GL_NORMAL_ARRAY);
			gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
			gl.glVertexPointer(3, GL10.GL_FLOAT, 0, m_MarkerVBuffer);
			gl.glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			gl.glDrawArrays(GL10.GL_LINE_LOOP, 0, 4);
			gl.glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			gl.glDrawArrays(GL10.GL_LINE_LOOP, 4, 4);
			gl.glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			for(int l=0;l<4;++l)
				gl.glDrawArrays(GL10.GL_LINE_STRIP, 8+l*2, 2);
		}
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
    
  //android.hardware.Camera.AutoFocusCallback
	public void onAutoFocus(boolean success, Camera camera)
	{
	}
    
	//com.baldzarika.ar.Tracker.Callback
    public void onRunningStateChanged(com.baldzarika.ar.Tracker tracker, int rs)
    {
    	m_Handler.removeCallbacks(m_HUDButtonsUpdateTask);
    	m_Handler.post(m_HUDButtonsUpdateTask);    	    	
    }
    
    public void onMarkerStateChanged(com.baldzarika.ar.Tracker.MarkerState markerState, int sc)
    {
    	if(m_ARSystem.isCorrect(markerState))
    	{
    		if(!m_IsDetected)
    			buildCube(markerState.getMarkerSize());
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
    		cameraParams.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
    		m_Camera.setParameters(cameraParams);
    		m_Camera.autoFocus(this);
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
			m_Camera.cancelAutoFocus();
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
	
	protected void onStartStopTracking()
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
	
	protected void buildCube(Size2 markerSize)
	{
		m_MarkerVBuffer.position(0);
		float halfW=0.5f*markerSize.m_Width;
		float halfL=0.5f*markerSize.m_Height;
		float height=(float)Math.sqrt(markerSize.m_Width*markerSize.m_Height);
		
		float [] lowCorners=
		{
			-halfW,  halfL, 0.0f,
			 halfW,  halfL, 0.0f,
			 halfW, -halfL, 0.0f,
			-halfW, -halfL, 0.0f
		};
		
		float [] highCorners=
		{
			-halfW,  halfL,  -height,
			 halfW,  halfL,  -height,
			 halfW, -halfL,  -height,
			-halfW, -halfL,  -height
		};
		
		m_MarkerVBuffer.put(lowCorners);
		m_MarkerVBuffer.put(highCorners);
		for(int h=0;h<4;++h)
		{
			m_MarkerVBuffer.put(lowCorners,h*3,3);			
			m_MarkerVBuffer.put(highCorners,h*3,3);
		}
		m_MarkerVBuffer.position(0);
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
    	public void run()
    	{
    		updateHUDButtons();
    	}
    };
    
    private FloatBuffer						m_MarkerVBuffer=ByteBuffer.allocateDirect((2*4+4*2)*3*4).order(ByteOrder.nativeOrder()).asFloatBuffer();
    
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
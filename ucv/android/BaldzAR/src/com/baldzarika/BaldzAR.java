package com.baldzarika;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.baldzarika.ar.Frame;
import com.baldzarika.ar.Marker;
import com.baldzarika.ar.Point2;
import com.baldzarika.ar.Size2;
import com.baldzarika.ar.Tracker;
import com.baldzarika.ar.Tracker.MarkerState;

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
import android.preference.PreferenceManager;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;

public class BaldzAR extends Activity implements Callback, PreviewCallback, Renderer, Tracker.Callback
{
	protected static final int SET_MARKER=1;
	protected static final int SETTINGS_CHANGED=2;
	
	//Activity
	@Override
    public void onCreate(Bundle savedInstanceState)
	{
    	super.onCreate(savedInstanceState);
    	
    	m_MarkerVBuffer=ByteBuffer.allocateDirect(4*3*4).order(ByteOrder.nativeOrder()).asFloatBuffer();
    	
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
    	
    	m_MarkerButton=(Button)findViewById(R.id.set_marker);
    	m_MarkerButton.setOnClickListener(new OnClickListener()
        	{ public void onClick(View v) { onSetMarker(); } }
        );
        
    	m_TrackingButton=(Button)findViewById(R.id.start_stop_tracking);
    	m_TrackingButton.setOnClickListener(new OnClickListener()
    		{ public void onClick(View v) { onStartStopTracking(); } }
        );
        
        m_SettingsButton=(Button)findViewById(R.id.settings);
        m_SettingsButton.setOnClickListener( new OnClickListener()
    		{ public void onClick(View v) { onSettings(); } }
        );
        
        Tracker tracker=BaldzARApp.getInstance().getTracker();
        m_MarkerLevel=(LevelIndicator)findViewById(R.id.marker_level);
        m_MarkerLevel.setRange(0.0f, tracker.getDetectionMinFeatures(), tracker.getTrackingMaxFeatures());
        
        BaldzARApp.getInstance().getTracker().setCallback(this);
        
        
        PreferenceManager.setDefaultValues(this, R.xml.settings, false);
        
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
		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();
		gl.glMatrixMode(GL10.GL_PROJECTION);
		gl.glLoadIdentity();
		gl.glOrthof(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 10.0f);
        
		gl.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
		
		MarkerState markerState=BaldzARApp.getInstance().getMarkerState();
		Tracker tracker=BaldzARApp.getInstance().getTracker();
		Size2 trackerFrameSize=tracker.getFrameSize();
		
		if(markerState.isDetected())
        {
			gl.glDisable(GL10.GL_CULL_FACE);
			gl.glDisable(GL10.GL_DEPTH_TEST);
			gl.glDisable(GL10.GL_BLEND);
			gl.glDisable(GL10.GL_LIGHTING);
                
			Point2[] marker_corners=markerState.getMarkerCorners();
                
			m_MarkerVBuffer.position(0);
			for(int c=0;c<4;++c)
			{
				float[] vertex=
				{
			    	((marker_corners[c].m_X/trackerFrameSize.m_Width)-0.5f)*2.0f,
			    	((marker_corners[c].m_Y/trackerFrameSize.m_Height)-0.5f)*(-2.0f),
			        0.0f
				};
				//Log.i("BaldzAR", "c.x=" + Float.toString(marker_corners[c].m_X) + " c.y=" + Float.toString(marker_corners[c].m_Y) );
			    m_MarkerVBuffer.put(vertex);
			}
			m_MarkerVBuffer.position(0);
			gl.glColor4f(1.0f, 0.0f, 0.0f, 0.33f);
			gl.glVertexPointer(3, GL10.GL_FLOAT, 0, m_MarkerVBuffer);
			gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
			gl.glDisableClientState(GL10.GL_COLOR_ARRAY);
			gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
			gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP, 0, 4);
		}
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
    	
    	m_MarkerButton.setEnabled(m_Camera!=null && !tracker.isStarted());
    	    	
    	m_TrackingButton.setEnabled(m_Camera!=null && markerSize.m_Width*markerSize.m_Height!=0);
    	m_TrackingButton.setBackgroundResource(tracker.isStarted()?R.drawable.stop_button:R.drawable.start_button);
    	
    	m_SettingsButton.setEnabled(m_Camera!=null && !tracker.isStarted());
    	
    }
    
    private void updateMarkerLevel()
    {
    	m_MarkerLevel.setValue(BaldzARApp.getInstance().getMarkerState().getFeatureMatchesSize());
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
    
    protected void settingsChanged()
    {
    	Tracker tracker=BaldzARApp.getInstance().getTracker();
        m_MarkerLevel.setRange(0.0f, tracker.getDetectionMinFeatures(), tracker.getTrackingMaxFeatures());
    }
    
    protected void onSetMarker()
	{
    	startActivityForResult(new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI), SET_MARKER);
    }
    
    protected void onStartStopTracking()
	{
    	Tracker tracker=BaldzARApp.getInstance().getTracker();
    	
    	if(tracker.isStarted())
    	{
    		tracker.stop();
    	}
    	else
    	{
    		tracker.start();
    	}
    	updateHUDButtons();
    }
    
    protected void onSettings()
    {
    	Intent launchSettingsIntent=new Intent().setClass(this, BaldzARSettings.class);
    	startActivityForResult(launchSettingsIntent, SETTINGS_CHANGED);
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
	
	private Handler							m_Handler=new Handler();
	private HUDButtonsUpdateTask			m_HUDButtonsUpdateTask=new HUDButtonsUpdateTask();
	private HUDMarkerLevelUpdateTask		m_HUDMarkerLevelUpdateTask=new HUDMarkerLevelUpdateTask();
	
	private SurfaceView                     m_CameraPreview=null;
    private GLSurfaceView           		m_GLView=null;
	private View							m_HUDView=null;
    private Camera                          m_Camera=null;
	private FloatBuffer						m_MarkerVBuffer=null;
	
	
	private Button							m_MarkerButton=null;
	private Button							m_TrackingButton=null;
	private Button							m_SettingsButton=null;
	private LevelIndicator					m_MarkerLevel=null;
	
   
}
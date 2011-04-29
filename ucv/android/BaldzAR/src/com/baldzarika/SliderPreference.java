package com.baldzarika;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.TypedArray;
import android.preference.Preference;
import android.util.AttributeSet;
import android.view.View;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

public class SliderPreference extends Preference implements OnSeekBarChangeListener
{
	public SliderPreference(Context context) 
	{
	    super(context);
	    setWidgetLayoutResource(R.layout.slider_preference);
	}
	
	public SliderPreference(Context context, AttributeSet attrs) 
	{
	    super(context, attrs);
	    setWidgetLayoutResource(R.layout.slider_preference);
	}
	
	public SliderPreference(Context context, AttributeSet attrs, int defStyle) 
	{
	    super(context, attrs, defStyle); 
	    setWidgetLayoutResource(R.layout.slider_preference);
	}
	
	@Override
    protected void onBindView(View view)
	{
        super.onBindView(view);
        
        m_Seek=(SeekBar)view.findViewById(R.id.slider_preference_slider);
        m_Text=(TextView)view.findViewById(R.id.slider_preference_text);
        
        m_Seek.setMax(m_MaxValue-m_MinValue);
        m_Seek.setProgress(m_Value-m_MinValue);
        m_Seek.setOnSeekBarChangeListener(this);
        m_Text.setText(m_Value+"");
    }
	
	@Override 
	public void onProgressChanged(SeekBar seekBar, int progress, boolean fromTouch) 
	{
		if(!callChangeListener(m_MinValue+progress))
	    {
			m_Seek.setProgress(m_Value-m_MinValue);
			return;
	    }
		m_Seek.setProgress(progress);    
	    m_Value=m_MinValue+progress;
	    m_Text.setText(m_Value+"");
	}
	
	@Override
	public void onStartTrackingTouch(SeekBar seekBar)
	{ 
		m_BeforeTouch=m_Seek.getProgress();
	}

	@Override 
	public void onStopTrackingTouch(SeekBar seekBar) 
	{
	    if(m_BeforeTouch!=m_Seek.getProgress())
	    {
	    	updatePreference(m_Value);
	    	notifyChanged();
	    }
	}
	
	@Override
	protected Object onGetDefaultValue(TypedArray ta, int index)
	{
		return validateValue(ta.getInt(index, m_MinValue));
	}

	@Override
	protected void onSetInitialValue(boolean restoreValue, Object defaultValue)
	{
	    int temp=restoreValue?getPersistedInt(m_MinValue):(Integer)defaultValue;
	    if(!restoreValue)
	    	persistInt(temp);
	    m_Value=temp;
	}

	private int validateValue(int value)
	{
		return Math.max(m_MinValue, Math.min(m_MaxValue, value));      
	}         

	private void updatePreference(int newValue)
	{
		SharedPreferences.Editor editor=getEditor();
		editor.putInt(getKey(), newValue);
		editor.commit();
	}
	
	public void setRange(int min, int max)
	{
		m_MinValue=min;
		m_MaxValue=max;
		m_Value=validateValue(m_Value);
		
		if(m_Seek!=null)
		{
			m_Seek.setMax(m_MaxValue-m_MinValue);
	        m_Seek.setProgress(m_Value-m_MinValue);
	        m_Text.setText(m_Value+"");
	        updatePreference(m_Value);
		}
    }
	
	private SeekBar m_Seek=null;
	private TextView m_Text=null;
	
	private int m_MaxValue=100;
	private int m_MinValue=0;
	private int m_Value=m_MinValue;
	private float m_BeforeTouch;
}

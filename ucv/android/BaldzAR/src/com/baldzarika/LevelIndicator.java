package com.baldzarika;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;

public class LevelIndicator extends View
{
	public LevelIndicator(Context context)
	{
		super(context);
	}
	
	public LevelIndicator(Context context, AttributeSet ats)
	{
		super(context, ats);
	}
	
	public LevelIndicator(Context context, AttributeSet ats, int defaultStyle)
	{
		super(context, ats, defaultStyle);
	}
	
	public void setRange(float min, float low, float max)
	{
		m_RangeMin=min;
		m_RangeLow=low;
		m_RangeMax=max;
		this.invalidate();
	}
	
	public void setValue(float value)
	{
		m_Value=value;
		this.invalidate();
	}
	
	@Override
	protected void onMeasure(int wMeasureSpec, int hMeasureSpec)
	{
		setMeasuredDimension(16, 240);
	}
	
	@Override
	protected void onDraw(Canvas canvas)
	{
		super.onDraw(canvas);
		
		int height=getMeasuredHeight();
		int width=getMeasuredWidth();
		
		int nLevels=height/8;
		
		Rect rcLed=new Rect(0,height-8,16,height);
		rcLed.inset(2, 1);
		
		float levelInc=(m_RangeMax-m_RangeMin)/nLevels;
		float lValue=m_RangeMin;
		for(int l=0;l<nLevels;++l)
		{
			lValue+=levelInc;
			
			boolean isLowLevel=Math.abs(lValue-m_RangeLow)<=(levelInc*0.5f);
			
			
			if(isLowLevel) rcLed.inset(-2, -1);
				
			
			if(lValue<=m_Value)
			{
				if(lValue<m_RangeLow)
				{
					float[] levelHSV={ 30.0f*(lValue-m_RangeMin)/(m_RangeLow-m_RangeMin) , 1.0f, 1.0f };
					m_Paint.setColor(Color.HSVToColor(192, levelHSV));					
				}
				else
				{
					float[] levelHSV={ 30.0f+90.0f*(lValue-m_RangeLow)/(m_RangeMax-m_RangeLow) , 1.0f, 1.0f };
					m_Paint.setColor(Color.HSVToColor(192, levelHSV));
				}
			}
			else
			{
				m_Paint.setColor(m_GrayLevelColor);
			}
			canvas.drawRect(rcLed, m_Paint);
			
			if(isLowLevel) rcLed.inset(2, 1);
			rcLed.offset(0, -8);
		}
	}
	
	private Paint	m_Paint=new Paint(Paint.ANTI_ALIAS_FLAG);
	private int		m_GrayLevelColor=Color.argb(192, Color.red(Color.GRAY), Color.green(Color.GRAY), Color.blue(Color.GRAY));
	private float	m_RangeMin=0.0f;
	private float	m_RangeLow=0.5f;
	private float	m_RangeMax=1.0f;
	private float	m_Value=0.5f;
}

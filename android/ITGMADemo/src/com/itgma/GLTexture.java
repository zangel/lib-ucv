package com.itgma;

import java.io.IOException;
import java.io.InputStream;

import javax.microedition.khronos.opengles.GL10;

import com.baldzarika.ar.Size2;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;



public class GLTexture
{
	public GLTexture()
	{
	}
	
	public boolean loadFromAsset(String fileName, Context ctx, GL10 gl)
	{
		AssetManager am=ctx.getAssets();
		try
		{
            InputStream bitmapStream=am.open(fileName);
            Bitmap texBitmap=BitmapFactory.decodeStream(bitmapStream);
            
            if(texBitmap==null)
            	return false;
            
            int [] texObj=new int[1];
    		gl.glGenTextures(1, texObj, 0);
    		m_TexObj=texObj[0];
    		if(m_TexObj<=0)
    			return false;
    		            
            m_TexSize.m_Width=texBitmap.getWidth();
            m_TexSize.m_Height=texBitmap.getHeight();
            
            gl.glBindTexture(GL10.GL_TEXTURE_2D, m_TexObj);
            gl.glTexParameterx(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
            gl.glTexParameterx(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
            
            gl.glTexParameterx(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE);
		    gl.glTexParameterx(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);
		    
            GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, texBitmap, 0);
            return true;
        }
		catch(IOException e){}
		return false;
	}
	
	public boolean isValid()
	{
		return m_TexObj>0 && m_TexSize.m_Width*m_TexSize.m_Height>0;
	}
	
	public int getTexObj()
	{
		return m_TexObj;
	}
	
	public void delete(GL10 gl)
	{
		if(m_TexObj>0)
		{
			int [] texObj=new int[1];
			texObj[0]=m_TexObj;
			gl.glDeleteTextures(1, texObj, 0);
		}
		m_TexSize.m_Width=m_TexSize.m_Height=0;
	}
		
	private int	m_TexObj=-1;
	private Size2 m_TexSize=new Size2(0,0);
}

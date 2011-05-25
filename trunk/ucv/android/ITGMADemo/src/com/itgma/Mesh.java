package com.itgma;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Vector;

import android.util.Log;

public class Mesh
{
	public Mesh(Vector<Float> coords, Vector<Float> normals, Vector<Float> texCoords,
			Vector<Short> coordPointer, Vector<Short> normalPointer, Vector<Short> texCoordPointer)
	{
		super();
		
		Log.i("FIDUCIAL_AR", 
				"CS="+Integer.toString(coords.size())+
				"TCS="+Integer.toString(texCoords.size())+
				"CPS="+Integer.toString(coordPointer.size())+
				"TCPS="+Integer.toString(texCoordPointer.size()));
				
				
		
		if(!coordPointer.isEmpty())
		{
			m_ArraySize=coordPointer.size();

			ByteBuffer coordsBuf=ByteBuffer.allocateDirect(m_ArraySize*3*4);
			coordsBuf.order(ByteOrder.nativeOrder());
			m_Coords=coordsBuf.asFloatBuffer();
			m_Coords.position(0);
			
			if(!normals.isEmpty() && normalPointer.size()==coordPointer.size())
			{
				ByteBuffer normalBuf=ByteBuffer.allocateDirect(coordPointer.size()*3*4);
				normalBuf.order(ByteOrder.nativeOrder());
				m_Normals=normalBuf.asFloatBuffer();
				m_Normals.position(0);
			}
			
			if(!texCoords.isEmpty() && texCoordPointer.size()==coordPointer.size())
			{
				ByteBuffer texCoordBuf=ByteBuffer.allocateDirect(coordPointer.size()*2*4);
				texCoordBuf.order(ByteOrder.nativeOrder());
				m_TexCoords=texCoordBuf.asFloatBuffer();
				m_TexCoords.position(0);
			}
			
			
			float [] vec3 = new float [3];
			float [] vec2 = new float [2];
			
			for(int i=0;i<coordPointer.size();++i)
			{
				vec3[0]=coords.get(coordPointer.get(i)*3+0);
				vec3[1]=coords.get(coordPointer.get(i)*3+1);
				vec3[2]=coords.get(coordPointer.get(i)*3+2);
				
				m_Coords.put(vec3);
				
				if(m_Normals!=null)
				{
					vec3[0]=normals.get(normalPointer.get(i)*3+0);
					vec3[1]=normals.get(normalPointer.get(i)*3+1);
					vec3[2]=normals.get(normalPointer.get(i)*3+2);
					m_Normals.put(vec3);
				}
				
				if(m_TexCoords!=null)
				{
					vec2[0]=texCoords.get(texCoordPointer.get(i)*2+0);
					vec2[1]=texCoords.get(texCoordPointer.get(i)*2+1);
					m_TexCoords.put(vec2);
				}
			}
			m_Coords.position(0);
			if(m_Normals!=null) m_Normals.position(0);
			if(m_TexCoords!=null) m_TexCoords.position(0);
		}
	}
	
	public boolean isEmpty()
	{
		return m_Coords==null;
	}
	
	public boolean hasNormals()
	{
		return m_Normals!=null;
	}
	
	public boolean hasTexCoords()
	{
		return m_TexCoords!=null;
	}
	
	public FloatBuffer getCoords()
	{
		return m_Coords;
	}
	
	public FloatBuffer getNormals()
	{
		return m_Normals;
	}
	
	public FloatBuffer getTexCoords()
	{
		return m_TexCoords;
	}
	
	public int getArraySize()
	{
		return m_ArraySize;
	}
	
	private int m_ArraySize=0;
	
	private FloatBuffer	m_Coords=null;
	private FloatBuffer m_Normals=null;
	private FloatBuffer m_TexCoords=null;
}

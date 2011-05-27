package com.itgma;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Vector;

public class MeshLoader
{
	public MeshLoader()
	{
	}
	
	public Mesh loadFromObjFile(String objFileName)
	{
		InputStream inputStream=null;
		try { inputStream=new FileInputStream(objFileName); }
		catch(IOException e) { return null; }
		return loadFromObjStream(inputStream);
	}
	
	public Mesh loadFromObjStream(InputStream objStream)
	{
		if(objStream==null) return null;
		
		m_CoordPtr.clear();
		m_TexCoordPtr.clear();
		m_NormalPtr.clear();
		m_Coords.clear();
		m_Normals.clear();
		m_TexCoords.clear();
		
		BufferedReader reader=new BufferedReader(new InputStreamReader(objStream));
		
		try
		{
			String line=null;
			
			while((line=reader.readLine())!=null)
			{
				if(line.startsWith("f"))
				{
					handleFLine(line);
				}
				else
				if(line.startsWith("vn"))
				{
					handleVNLine(line);
				}
				else
				if(line.startsWith("vt"))
				{
					handleVTLine(line);
				}
				else
				if(line.startsWith("v"))
				{
					handleVLine(line);
				}
			}
		}
		catch(IOException e)
		{
			return null;
		}
		
		return new Mesh(m_Coords, m_Normals, m_TexCoords, m_CoordPtr, m_NormalPtr, m_TexCoordPtr);
	}
	
	private void handleVLine(String line)
	{
		String [] tokens=line.split("[ ]+");
		int c=Math.min(tokens.length,4);
		for(int i=1;i<c;++i)
			m_Coords.add(Float.valueOf(tokens[i]));
	}

	private void handleVNLine(String line)
	{
		String [] tokens=line.split("[ ]+");
		int c=Math.min(tokens.length,4);
		for(int i=1;i<c;++i)
			m_Normals.add(Float.valueOf(tokens[i]));
	}
	
	private void handleVTLine(String line)
	{
		String [] tokens=line.split("[ ]+");
		int c=Math.min(tokens.length,3);
		for(int i=1;i<c;++i)
			m_TexCoords.add(Float.valueOf(tokens[i]));
	}

	private void handleFLine(String line)
	{
		String [] tokens=line.split("[ ]+");
		int c=tokens.length;
		if(tokens[1].matches("[0-9]+") && false)
		{
			if(c==4)
			{
				for(int i=1;i<c;++i)
				{
					Short s=Short.valueOf(tokens[i]); s--;
					m_CoordPtr.add(s);
				}
			}
			else
			{
				Vector<Short> polygon=new Vector<Short>();
				for(int i=1;i<c;i++)
				{
					Short s=Short.valueOf(tokens[i]); s--;
					polygon.add(s);
				}
				m_CoordPtr.addAll(triangulate(polygon));
			}
		}
		else
		if(tokens[1].matches("[0-9]+/[0-9]+"))
		{
			if(c==4)
			{
				for(int i=1;i<c;++i)
				{
					String [] v_vt_tokens=tokens[i].split("/");
					
					Short s=Short.valueOf(v_vt_tokens[0]); s--;
					m_CoordPtr.add(s);
					
					s=Short.valueOf(v_vt_tokens[1]); s--;
					m_TexCoordPtr.add(s);
				}
			}
			else
			{
				Vector<Short> tmpCoordPtr=new Vector<Short>();
				Vector<Short> tmpTexCoordPtr=new Vector<Short>();
				for(int i=1;i<c;++i)
				{
					String [] v_vt_tokens=tokens[i].split("/");
					
					Short s=Short.valueOf(v_vt_tokens[0]); s--;
					tmpCoordPtr.add(s);
					
					s=Short.valueOf(v_vt_tokens[1]); s--;
					tmpTexCoordPtr.add(s);
				}
				m_CoordPtr.addAll(triangulate(tmpCoordPtr));
				m_TexCoordPtr.addAll(triangulate(tmpTexCoordPtr));
			}
		}
		else
		if(tokens[1].matches("[0-9]+/[0-9]+/[0-9]+") && false)
		{
			if(c==4)
			{
				for(int i=1;i<c;i++)
				{
					String [] v_vt_vn_tokens=tokens[i].split("/");
					
					Short s=Short.valueOf(v_vt_vn_tokens[0]); s--;
					m_CoordPtr.add(s);
					
					s=Short.valueOf(v_vt_vn_tokens[1]); s--;
					m_TexCoordPtr.add(s);
					
					s=Short.valueOf(v_vt_vn_tokens[2]); s--;
					m_NormalPtr.add(s);
				}
			}
			else
			{
				Vector<Short> tmpCoordPtr=new Vector<Short>();
				Vector<Short> tmpTexCoordPtr=new Vector<Short>();
				Vector<Short> tmpNormalPtr=new Vector<Short>();
				for(int i=1; i<tokens.length; i++)
				{
					String [] v_vt_vn_tokens=tokens[i].split("/");
					
					Short s=Short.valueOf(v_vt_vn_tokens[0]); s--;
					tmpCoordPtr.add(s);
					
					s=Short.valueOf(v_vt_vn_tokens[1]); s--;
					tmpTexCoordPtr.add(s);
					
					s=Short.valueOf(v_vt_vn_tokens[2]); s--;
					tmpNormalPtr.add(s);
				}
				m_CoordPtr.addAll(triangulate(tmpCoordPtr));
				m_TexCoordPtr.addAll(triangulate(tmpTexCoordPtr));
				m_NormalPtr.addAll(triangulate(tmpNormalPtr));
			}
		}
	}

	private static Vector<Short> triangulate(Vector<Short> polygon)
	{
		Vector<Short> triangles=new Vector<Short>();
		for(int i=1; i<polygon.size()-1; i++)
		{
			triangles.add(polygon.get(0));
			triangles.add(polygon.get(i));
			triangles.add(polygon.get(i+1));
		}
		return triangles;
	}

	private Vector<Short> m_CoordPtr=new Vector<Short>();
	private Vector<Short> m_TexCoordPtr=new Vector<Short>();
	private Vector<Short> m_NormalPtr=new Vector<Short>();
	
	private Vector<Float> m_Coords=new Vector<Float>();
	private Vector<Float> m_Normals=new Vector<Float>();
	private Vector<Float> m_TexCoords=new Vector<Float>();
}

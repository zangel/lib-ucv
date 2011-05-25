package com.itgma;

import java.util.List;

import com.baldzarika.ar.Size2;

import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;

public class DemoSettings extends PreferenceActivity
{
	@Override
    protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.xml.settings);
		ListPreference frameSizeSetting = (ListPreference) findPreference(getString(R.string.settings_frame_size_key));
		
		List<Size2> frameSizes=DemoApp.getInstance().getFrameSizes();
		
		CharSequence[] fsEntries=new CharSequence[frameSizes.size()];
		CharSequence[] fsEntryValues=new CharSequence[frameSizes.size()];
		
		for(int i=0;i<frameSizes.size();i++)
		{
			StringBuilder sb=new StringBuilder();
			sb.
				append(frameSizes.get(i).m_Width).
				append("x").
				append(frameSizes.get(i).m_Height);
			fsEntries[i]=fsEntryValues[i]=sb.toString();
		}
		
		frameSizeSetting.setEntries(fsEntries);
		frameSizeSetting.setEntryValues(fsEntryValues);
	}
	
}

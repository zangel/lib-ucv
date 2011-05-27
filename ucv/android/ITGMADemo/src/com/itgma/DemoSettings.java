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
		ListPreference frameSizeSetting = (ListPreference) findPreference(getString(R.string.settings_preview_size_key));
		
		List<Size2> previewSizes=DemoApp.getInstance().getPreviewSizes();
		
		CharSequence[] fsEntries=new CharSequence[previewSizes.size()];
		CharSequence[] fsEntryValues=new CharSequence[previewSizes.size()];
		
		for(int i=0;i<previewSizes.size();i++)
		{
			StringBuilder sb=new StringBuilder();
			sb.
				append(previewSizes.get(i).m_Width).
				append("x").
				append(previewSizes.get(i).m_Height);
			fsEntries[i]=fsEntryValues[i]=sb.toString();
		}
		
		frameSizeSetting.setEntries(fsEntries);
		frameSizeSetting.setEntryValues(fsEntryValues);
	}
	
}

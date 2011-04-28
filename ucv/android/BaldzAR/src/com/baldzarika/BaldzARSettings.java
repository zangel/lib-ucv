package com.baldzarika;

import java.util.List;

import com.baldzarika.ar.Size2;

import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;

public class BaldzARSettings extends PreferenceActivity
{
	@Override
    protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.xml.settings);
		ListPreference frameSizeSetting = (ListPreference) findPreference(getString(R.string.settings_frame_size_key));
		
		List<Size2> frameSizes=BaldzARApp.getInstance().getFrameSizes();
		
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
		
		
		SliderPreference detectionTreshold=(SliderPreference) findPreference(getString(R.string.settings_detection_treshold_key));
		detectionTreshold.setRange(0, 100);
		
		SliderPreference detectionMinFeatures=(SliderPreference) findPreference(getString(R.string.settings_detection_min_features_key));
		detectionMinFeatures.setRange(8, 16);
		
		SliderPreference trackingMaxFeatures=(SliderPreference) findPreference(getString(R.string.settings_tracking_max_features_key));
		trackingMaxFeatures.setRange(8, 16);
		
		SliderPreference trackingWinSize=(SliderPreference) findPreference(getString(R.string.settings_tracking_win_size_key));
		trackingWinSize.setRange(5, 10);
	}
}

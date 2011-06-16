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

		ListPreference previewSizeSetting = (ListPreference) findPreference(getString(R.string.settings_general_preview_size_key));
		List<Size2> previewSizes=BaldzARApp.getInstance().getPreviewSizes();
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
		previewSizeSetting.setEntries(fsEntries);
		previewSizeSetting.setEntryValues(fsEntryValues);
		
		ListPreference arSystemSettings = (ListPreference) findPreference(getString(R.string.settings_general_system_key));
		
		CharSequence[] arSystemEntries=new CharSequence[2];
		CharSequence[] arSystemValues=new CharSequence[2];
		arSystemEntries[0]=arSystemValues[0]=ARSystem.AR_FIDUCIAL_SYSTEM;
		arSystemEntries[1]=arSystemValues[1]=ARSystem.AR_MARKERLESS_SYSTEM;
		arSystemSettings.setEntries(arSystemEntries);
		arSystemSettings.setEntryValues(arSystemValues);
	}
}

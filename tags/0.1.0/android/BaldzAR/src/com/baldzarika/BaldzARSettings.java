package com.baldzarika;

import java.util.List;
import com.baldzarika.ar.Size2;

import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;

public class BaldzARSettings extends PreferenceActivity
{
	private static final int ACTIVITY_SELECT_MARKER_IMAGE=1;
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
		
		m_MarkerPreference=(MarkerPreference)findPreference(getString(R.string.settings_markerless_marker_key));
		m_MarkerPreference.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener()
		{
			public boolean onPreferenceClick(Preference preference)
			{
            	selectMarkerImage();
                return true;
            }
        });
        
	}
	
	protected void selectMarkerImage()
	{
        Intent i = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.INTERNAL_CONTENT_URI); 
        startActivityForResult(i, ACTIVITY_SELECT_MARKER_IMAGE);
    }
	
	@Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent)
	{ 
        super.onActivityResult(requestCode, resultCode, intent);
        switch(requestCode)
        { 
            case ACTIVITY_SELECT_MARKER_IMAGE:
            	if(resultCode==RESULT_OK)
            	{
            		Uri imageURI = intent.getData();
            		SharedPreferences prefs=PreferenceManager.getDefaultSharedPreferences(getBaseContext());
            		SharedPreferences.Editor editor=prefs.edit();
            		editor.putString(getString(R.string.settings_markerless_marker_key), imageURI.toString());
            		editor.commit();
            		m_MarkerPreference.updateMarkerImage();
                }
                break;
        }
    }
	
	private MarkerPreference m_MarkerPreference=null;
}

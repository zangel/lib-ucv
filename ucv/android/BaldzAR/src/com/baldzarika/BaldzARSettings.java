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
		detectionTreshold.setRange(BaldzARApp.MIN_DETECTION_TRESHOLD, BaldzARApp.MAX_DETECTION_TRESHOLD);
		
		SliderPreference detectionMinFeatures=(SliderPreference) findPreference(getString(R.string.settings_detection_min_features_key));
		detectionMinFeatures.setRange(BaldzARApp.MIN_DETECTION_MIN_FEATURES, BaldzARApp.MAX_DETECTION_MIN_FEATURES);
		
		SliderPreference detectionMaxDiffNorm=(SliderPreference) findPreference(getString(R.string.settings_detection_max_diff_norm_key));
		detectionMaxDiffNorm.setRange(BaldzARApp.MIN_DETECTION_MAX_DIFF_NORM, BaldzARApp.MAX_DETECTION_MAX_DIFF_NORM);
				
		SliderPreference trackingHalfWinSize=(SliderPreference) findPreference(getString(R.string.settings_tracking_half_win_size_key));
		trackingHalfWinSize.setRange(BaldzARApp.MIN_TRACKING_HALF_WIN_SIZE, BaldzARApp.MAX_TRACKING_HALF_WIN_SIZE);
		
		SliderPreference trackingNumLevels=(SliderPreference) findPreference(getString(R.string.settings_tracking_num_levels_key));
		trackingNumLevels.setRange(BaldzARApp.MIN_TRACKING_NUM_LEVELS, BaldzARApp.MAX_TRACKING_NUM_LEVELS);
		
		SliderPreference trackingMaxIterations=(SliderPreference) findPreference(getString(R.string.settings_tracking_max_iterations_key));
		trackingMaxIterations.setRange(BaldzARApp.MIN_TRACKING_MAX_ITERATIONS, BaldzARApp.MAX_TRACKING_MAX_ITERATIONS);
		
		SliderPreference trackingMaxFeatures=(SliderPreference) findPreference(getString(R.string.settings_tracking_max_features_key));
		trackingMaxFeatures.setRange(BaldzARApp.MIN_TRACKING_MAX_FEATURES, BaldzARApp.MAX_TRACKING_MAX_FEATURES);
		
		SliderPreference trackingMaxDiffNorm=(SliderPreference) findPreference(getString(R.string.settings_tracking_max_diff_norm_key));
		trackingMaxDiffNorm.setRange(BaldzARApp.MIN_TRACKING_MAX_DIFF_NORM, BaldzARApp.MAX_TRACKING_MAX_DIFF_NORM);
	}
}

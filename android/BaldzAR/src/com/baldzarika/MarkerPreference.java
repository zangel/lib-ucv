package com.baldzarika;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.preference.Preference;
import android.preference.PreferenceManager;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ImageView;

public class MarkerPreference extends Preference
{
	ImageView imageView;

	public MarkerPreference(Context context, AttributeSet attrs)
	{
		super(context, attrs);
    }
        
	@Override
    public void onBindView(View view)
	{
        super.onBindView(view);
        imageView = (ImageView)view.findViewById(R.id.pref_marker_image);
        updateMarkerImage();
    }

    void updateMarkerImage()
    {
    	if(imageView!=null)
    	{
            Bitmap bitmap=null;
            SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getContext());
            String imageURIString = prefs.getString(
            	getContext().getString(R.string.settings_markerless_marker_key),
            	null
            );
            if (imageURIString!=null)
            {
            	Uri imageURI = Uri.parse(imageURIString);
            	try
            	{
                	BitmapFactory.Options options = new BitmapFactory.Options();
                    options.inJustDecodeBounds=true;
                    BitmapFactory.decodeStream(getContext().getContentResolver().openInputStream(imageURI), null, options);
                    options.inJustDecodeBounds=false;
                    
                    float wratio=1.0f*options.outWidth / Math.max(128, imageView.getWidth());
                    float hratio=1.0f*options.outHeight / Math.max(128, imageView.getHeight());         
                    options.inSampleSize = (int)Math.min(wratio, hratio);
                    
                    bitmap=BitmapFactory.decodeStream(getContext().getContentResolver().openInputStream(imageURI), null, options);  
                    
                }
                catch(Exception ignored){}
            }
            imageView.setImageBitmap(bitmap);
    	}
    }
        

}
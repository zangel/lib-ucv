package com.baldzarika;

import android.app.Activity;
import android.os.Bundle;

import com.baldzarika.ar.*;

public class BaldzAR extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        m_Frame=new Frame();
        
        int width=m_Frame.getSize().m_Width;
        int height=m_Frame.getSize().m_Height;
        m_Frame.dispose();
    }
    
    private Frame m_Frame;
}
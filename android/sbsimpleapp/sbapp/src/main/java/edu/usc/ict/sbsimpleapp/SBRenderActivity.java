package edu.usc.ict.sbsimpleapp;


import android.app.Activity;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import edu.usc.ict.sbmobile.SBLoadingTask;
import edu.usc.ict.sbmobile.SBMobileLib;
import edu.usc.ict.sbmobile.SBMobileSurfaceView;
import android.view.View;
import android.util.Log;

public class SBRenderActivity extends ActionBarActivity {
    protected SBMobileSurfaceView _sbview = null;
    private Handler updateHandler;

    private class MyRunnable implements Runnable
    {
        private Handler myHandler;

        MyRunnable(Handler handler)
        {
            super();
            myHandler = handler;
        }

        @Override
        public void run() {
            SBMobileLib.step();
            myHandler.postDelayed(this, 16);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
		Log.d("SB",  "The onCreate() event");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_vhrender);


        // Init SmartBody via JNI functions in SBMobileLib
        _sbview = (SBMobileSurfaceView)findViewById(R.id.vhview);
        _sbview.sbReloadTexture = true;
        final String mediaDir = getResources().getString(R.string.media_dir);
        final SBLoadingTask loadingTask = loadSBData(mediaDir);
        updateHandler = new Handler();

        // create a runnable that waits for the files to be loaded
        updateHandler.postDelayed(new Runnable()
        {
            @Override
            public void run()
            {
                if (!loadingTask.isDone())
                {
                    Log.d("SBM", "Loading task not finished...");
                    updateHandler.postDelayed(this, 500);
                }
                else
                {
                    Log.d("SBM", "Files now loaded to /sdcard, now setting up VHMobile...");
                    SBMobileLib.setup("/sdcard/"+ mediaDir + "/");
                    Log.d("SBM", "Now initting VHMobile...");
                    SBMobileLib.init();
                    Log.d("SBM", "VHMobile init finished.");
                    updateHandler.removeCallbacks(this);
                    updateHandler.postDelayed(new MyRunnable(updateHandler), 30);
                }
            }
        }, 100);
    }


    private SBLoadingTask loadSBData(String mediaDir)
    {
        SBLoadingTask loadingTask = new SBLoadingTask((Activity)this, mediaDir);
        loadingTask.execute();
        return loadingTask;
    }

    @Override
    protected void onStart()
    {
		Log.d("SB",  "The onStart() event");
        super.onStart();

    }

    @Override
    protected void onResume() {
        super.onResume();
        /*
         * The activity must call the GL surface view's
         * onResume() on activity onResume().
         */
       if (_sbview != null)
           _sbview.onResume();
	    Log.d("SB", "The onResume() event");
    }

    @Override
    protected void onPause() {
        super.onPause();
        /*
         * The activity must call the GL surface view's
         * onResume() on activity onResume().
         */
        if (_sbview != null)
            _sbview.onPause();
		 Log.d("SB", "The onPause() event");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
	

   /** Called when the activity is no longer visible. */
   @Override
   protected void onStop() {
      super.onStop();
      Log.d("SB", "The onStop() event");
   }

   /** Called just before the activity is destroyed. */
   @Override
   public void onDestroy() {
      super.onDestroy();
      Log.d("SB", "The onDestroy() event");
   }
}

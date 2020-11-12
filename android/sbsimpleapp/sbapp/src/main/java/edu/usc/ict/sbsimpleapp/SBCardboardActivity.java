package edu.usc.ict.sbsimpleapp;

import android.content.Context;
import android.os.Bundle;
import android.os.Vibrator;
import android.view.Menu;
import android.view.MenuItem;

import com.google.vrtoolkit.cardboard.CardboardActivity;
import edu.usc.ict.sbmobile.SBMobileCardboardView;
import edu.usc.ict.sbmobile.SBMobileLib;

import com.google.vrtoolkit.cardboard.CardboardView;


public class SBCardboardActivity extends CardboardActivity {
    protected CardboardView cardboardView = null;
    private Vibrator vibrator;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cardboard);

    }

    @Override
    protected void onStart()
    {
        super.onStart();
        cardboardView = (SBMobileCardboardView) findViewById(R.id.cardboardView);
        //cardboardView.setPreserveEGLContextOnPause(true);
        //cardboardView.setRenderer(new VHCardboardRenderer());
        setCardboardView(cardboardView);
        vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
    }

    @Override
    protected void onResume() {
        super.onResume();
        /*
         * The activity must call the GL surface view's
         * onResume() on activity onResume().
         */
        if (cardboardView != null) {
            cardboardView.onResume();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        /*
         * The activity must call the GL surface view's
         * onResume() on activity onResume().
         */
        if (cardboardView != null) {
            cardboardView.onPause();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public void onCardboardTrigger() {
        //VHMobileLib.widgetOnClick("cardboard_button");
        vibrator.vibrate(50);
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
}

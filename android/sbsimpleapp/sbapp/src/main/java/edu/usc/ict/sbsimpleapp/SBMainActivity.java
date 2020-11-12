package edu.usc.ict.sbsimpleapp;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import edu.usc.ict.sbmobile.SBMobileLib;
import edu.usc.ict.sbmobile.SBLoadingTask;

public class SBMainActivity extends Activity {

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

	}



	@Override
	protected void onStart()
	{
		super.onStart();
		String sbRenderer = SBMobileLib.getStringAttribute("SBMobileRenderer");
		Log.d("SB", "VHRenderer Attribute = " + sbRenderer);
		if (sbRenderer.equals("Cardboard"))
		{
			finish();
			Intent myIntent = new Intent(this, SBCardboardActivity.class);
			myIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			this.startActivity(myIntent);
		}
		else if (sbRenderer.equals("GLRender"))
		{
			finish();
			Intent myIntent = new Intent(this, SBRenderActivity.class);
			myIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			this.startActivity(myIntent);
		}
		else
		{
			finish();
			Intent myIntent = new Intent(this, SBRenderActivity.class);
			myIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			this.startActivity(myIntent);
		}
	}
}

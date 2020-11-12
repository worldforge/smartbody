package edu.usc.ict.sbmobile;

import android.app.Activity;
import android.os.AsyncTask;
import android.app.ProgressDialog;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class SBLoadingTask extends AsyncTask<Void, Integer, Boolean> {

	final static String TARGET_BASE_PATH = Environment.getExternalStorageDirectory().getPath() + "/";
	Activity thisActivity;
	ProgressDialog dialog;
	boolean hasCopiedAssets;
	boolean hasFinished;
	String finalLoadingPath;
	public SBLoadingTask(Activity activity, String mediaDir)
	{
		hasFinished = false;
		thisActivity = activity;
		finalLoadingPath = TARGET_BASE_PATH + mediaDir;
		File dir = new File(finalLoadingPath);
		if (dir.exists())
			hasCopiedAssets = true;
		else
			hasCopiedAssets = false;
	}

	@Override
	protected Boolean doInBackground(Void... params) {

		try {
			copyFilesToSdCard();
		}
		catch (Exception e)
		{
			return false;
		}
		return true;
	}

	@Override
	protected void onPostExecute(Boolean result) {
		super.onPostExecute(result);
		//Toast.makeText(thisActivity,
		//		"Progress Ends",
		//		Toast.LENGTH_LONG).show();

		dialog.dismiss();
		
		// indicate that loading has been completed
		hasFinished = true;
		
	}
	
	public boolean isDone()
	{
		return hasFinished;
	}

	@Override
	protected void onPreExecute() {
		super.onPreExecute();
		//Toast.makeText(thisActivity,
		//		"Progress Begins",
		//		Toast.LENGTH_LONG).show();
		if (!hasCopiedAssets)
		{
			dialog = ProgressDialog.show(thisActivity, "Please wait ...",
					"Unpacking and Loading resources ...", true);
			dialog.setCancelable(false);
		}
		else
		{
			dialog = ProgressDialog.show(thisActivity, "Please wait ...",
					"Starting app...", true);
			dialog.setCancelable(false);
		}


	}

	@Override
	protected void onProgressUpdate(Integer... values) {
	}


	private void copyFilesToSdCard() {
		/*
		 * ProgressDialog progress = new ProgressDialog(this);
		 * progress.setTitle("Loading");
		 * 
		 * CharSequence text = "Upacking Resources..."; int duration =
		 * Toast.LENGTH_LONG;
		 * 
		 * Toast inputToast = Toast.makeText(this, text, duration);
		 * inputToast.show();
		 * 
		 * progress.setMessage("Wait while loading..."); progress.show();
		 */
		File dir = new File(finalLoadingPath);
		if (dir.exists())
			return;
		copyFileOrDir(""); // copy all files in assets folder in my project

		// To dismiss the dialog
		// progress.dismiss();
	}

	private void copyFileOrDir(String path) {
		AssetManager assetManager = thisActivity.getAssets();
		String assets[] = null;
		try {
			Log.i("tag", "copyFileOrDir() " + path);
			assets = assetManager.list(path);
			if (assets.length == 0) {
				copyFile(path);
			} else {
				String fullPath = TARGET_BASE_PATH + path;
				Log.i("tag", "path=" + fullPath);
				File dir = new File(fullPath);

				if (!dir.exists() && !path.startsWith("images")
						&& !path.startsWith("sounds")
						&& !path.startsWith("webkit")) {
					if (dir.exists())
						return;
					if (!dir.mkdirs())
						Log.i("tag", "could not create dir " + fullPath);
				}
				for (int i = 0; i < assets.length; ++i) {
					String p;
					if (path.equals(""))
						p = "";
					else
						p = path + "/";

					if (!path.startsWith("images")
							&& !path.startsWith("sounds")
							&& !path.startsWith("webkit"))
						copyFileOrDir(p + assets[i]);
				}
			}
		} catch (IOException ex) {
			Log.e("tag", "I/O Exception", ex);
		}
	}

	private void copyFile(String filename) {
		AssetManager assetManager = thisActivity.getAssets();

		InputStream in = null;
		OutputStream out = null;
		String newFileName = null;
		try {
			Log.i("tag", "copyFile() " + filename);
			in = assetManager.open(filename);
			if (filename.endsWith(".jpg")) // extension was added to avoid
											// compression on APK file
				newFileName = TARGET_BASE_PATH
						+ filename.substring(0, filename.length() - 4);
			else
				newFileName = TARGET_BASE_PATH + filename;
			out = new FileOutputStream(newFileName);

			byte[] buffer = new byte[1024];
			int read;
			while ((read = in.read(buffer)) != -1) {
				out.write(buffer, 0, read);
			}
			in.close();
			in = null;
			out.flush();
			out.close();
			out = null;
		} catch (Exception e) {
			Log.e("tag", "Exception in copyFile() of " + newFileName);
			Log.e("tag", "Exception in copyFile() " + e.toString());
		}

	}
}

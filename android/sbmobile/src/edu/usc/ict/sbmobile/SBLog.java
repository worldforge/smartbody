package edu.usc.ict.sbmobile;

import android.util.Log;

/**
 * Created by feng on 2017/9/8.
 */

public class SBLog {
    static public void e(String tag, String message)
    {
        Log.e(tag, message);
    }

    static public void e(String tag, String message, Throwable e)
    {
        Log.e(tag, message, e);
    }

    static public void d(String tag, String message)
    {
        Log.d(tag, message);
    }

    static public void v(String tag, String message)
    {
        Log.v(tag, message);
    }

    static public void i(String tag, String message)
    {
        Log.i(tag, message);
    }

    static public void w(String tag, String message)
    {
        Log.w(tag, message);
    }
}

package edu.usc.ict.sbmobile;

import android.util.Log;

import com.google.vrtoolkit.cardboard.CardboardView;
import com.google.vrtoolkit.cardboard.Eye;
import com.google.vrtoolkit.cardboard.HeadTransform;
import com.google.vrtoolkit.cardboard.Viewport;

import javax.microedition.khronos.egl.EGLConfig;


public class SBCardboardRenderer implements CardboardView.StereoRenderer{
    private float[] eyeView;
    @Override
    public void onSurfaceCreated(EGLConfig config) {

    }

    @Override
    public void onRendererShutdown() {
        Log.i("SBM", "onRendererShutdown");
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        Log.i("SBM", "onSurfaceChanged");
    }

    @Override
    public void onNewFrame(HeadTransform headTransform) {

    }
    @Override
    public void onDrawEye(Eye eye) {
        if (SBMobileCardboardView.sbReloadTexture)
        {
            SBMobileLib.reloadTexture();
            SBMobileCardboardView.sbReloadTexture = false;
        }
        eyeView = eye.getEyeView();
        SBMobileLib.renderCardboard(eyeView);
    }

    @Override
    public void onFinishFrame(Viewport viewport) {
    }
}

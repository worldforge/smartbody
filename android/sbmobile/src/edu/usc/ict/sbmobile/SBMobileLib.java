/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package edu.usc.ict.sbmobile;

// Wrapper for native library

import android.media.MediaPlayer;
import android.util.Log;

public class SBMobileLib {

    public static abstract class AudioCallback
    {
        public abstract void onAudioPlayback(String soundFile);
    }

    public static interface EngineCallback
    {
        public abstract void onEngineCallback(String params);
    }

     static MediaPlayer _mediaPlayer = null;
    static AudioCallback _audioCallback = null;
    static EngineCallback _engineCallback = null;

     static {
	//System.loadLibrary("python2.7");
	System.loadLibrary("sbmobile");
     }

    /**
     * @param width the current view width
     * @param height the current view height
     */

     public static native void setup(String mediaPath);
     public static native void init();
     public static native void openConnection();
     public static native void closeConnection();

     public static native boolean handleInputEvent(int action, float x, float y);

     public static native void executeSB(String sbmCmd);
     public static native String getLog();
     public static native void step();
     public static native void render(float [] modelViewMat);
     public static native void renderAR(float [] modelViewMat, float [] projMat, int updateGaze);
     public static native void renderFBOTex(int width, int height, int texID);
     public static native void renderCardboard(float eyeView[]);
     public static native void reloadTexture();
     public static native void surfaceChanged(int width, int height);

     public static native String  getStringAttribute(String attrName);
	 public static native boolean  getBoolAttribute(String attrName);
	 public static native double  getDoubleAttribute(String attrName);
	 public static native int getIntAttribute(String attrName);
     public static native void setBackground(String fileName, String textureName, int texRotate);
     public static native void createCustomMeshFromBlendshapes(String templateMeshName, String blendshapesDir, String baseMeshName, String hairMeshName, String outMeshName);
     public static native void imageColorTransfer(String srcImag, String srcMask, String tgtImg, String tgtMask, String outImage);
     public static native void deformableMeshTextureReplace(String meshName, String texName, String imgFileName);
     public static native void replaceSubMesh(String meshName, String subMeshName, String inputMeshName);




    public static native void initClassifier(String classifierName, String dialogManagerName);
     public static native String classify(String characterName, String question);
     public static native String [] classifyWithExternalID(String characterName, String question);

    public static void playSound(String soundFile, boolean loopSound)
    {
        Log.d("SBM", "SBMobileLib: Playing sound " + soundFile);
        if (_mediaPlayer != null)
        {
            stopSound();
        }
        try {
            _mediaPlayer = new MediaPlayer();
            _mediaPlayer.setDataSource(soundFile);
            _mediaPlayer.prepare();
            if (loopSound)
                _mediaPlayer.setLooping(true);
            _mediaPlayer.start();
            if (_audioCallback != null)
            {
                _audioCallback.onAudioPlayback(soundFile);
            }

        } catch (Exception e) {
            Log.e("SBM", "SBMobileLib: Problem playing sound " + soundFile);
            e.printStackTrace();
        }
    }

    public static void stopSound() {
        Log.d("SBM", "SBMobileLib: Stop playing sound ");
        if (_mediaPlayer != null) {
            if (_mediaPlayer.isPlaying())
                _mediaPlayer.stop();
            _mediaPlayer.release();
            _mediaPlayer = null;
        }
    }

    public static void engineCallback(String params)
    {
            Log.d("SBM", "engineCallback: " + params);
            if (_engineCallback != null)
            {
                _engineCallback.onEngineCallback(params);
            }
    }

    public static void setAudioCallback(AudioCallback callback)
    {
        _audioCallback = callback;
    }

    public static void setEngineCallback(EngineCallback callback)
    {
        _engineCallback = callback;
    }

}

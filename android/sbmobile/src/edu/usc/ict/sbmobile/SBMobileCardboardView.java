
package edu.usc.ict.sbmobile;
/*
 * Copyright (C) 2008 The Android Open Source Project
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

import android.content.Context;
import android.util.AttributeSet;

import com.google.vrtoolkit.cardboard.CardboardView;


public class SBMobileCardboardView extends CardboardView {
    private static String TAG = "SBM";
    public static boolean sbReloadTexture = false;
    
    public SBMobileCardboardView(Context context) {
        super(context);
        init(false, 0, 0, context);
    }
    
    public SBMobileCardboardView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        init(false, 0, 0, context);
    }
   
    public SBMobileCardboardView(Context context, boolean translucent, int depth, int stencil) {
        super(context);
        init(translucent, depth, stencil, context);
    }

    
    @Override
     public void onResume() {
        super.onResume();
        sbReloadTexture = true;
        //Log.d("SBM","VHMobileSurfaceView::onResume");
    }

    @Override
    public void onPause() {
        super.onPause();
        //Log.d("SBM", "VHMobileSurfaceView::onPause");

    }

    private void init(boolean translucent, int depth, int stencil, Context context) {

        /* By default, GLSurfaceView() creates a RGB_565 opaque surface.
         * If we want a translucent one, we should change the surface's
         * format here, using PixelFormat.TRANSLUCENT for GL Surfaces
         * is interpreted as any 32-bit surface with alpha by SurfaceFlinger.
         */
        setRenderer(new SBCardboardRenderer());
        setPreserveEGLContextOnPause(true);

    }
}

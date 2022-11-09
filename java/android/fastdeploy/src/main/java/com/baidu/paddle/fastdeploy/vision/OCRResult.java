package com.baidu.paddle.fastdeploy.vision;

import android.support.annotation.NonNull;

import com.baidu.paddle.fastdeploy.FastDeployInitializer;

import java.util.Arrays;

public class OCRResult {
    public int[][] mBoxes;  // [n,8]
    public String[] mText;  // [n]
    public float[] mRecScores;  // [n]
    public float[] mClsScores;  // [n]
    public int[] mClsLabels;  // [n]
    public boolean mInitialized = false;

    public OCRResult() {
        mInitialized = false;
    }

    public OCRResult(long nativeResultContext) {
        mInitialized = copyAllFromNativeContext(nativeResultContext);
    }

    public boolean initialized() {
        return mInitialized;
    }

    // Setup results from native buffers.
    private boolean copyAllFromNativeContext(long nativeResultContext) {
        if (nativeResultContext == 0) {
            return false;
        }
        if (copyBoxesNumFromNative(nativeResultContext) > 0) {
            setBoxes(copyBoxesFromNative(nativeResultContext));
            setText(copyTextFromNative(nativeResultContext));
            setRecScores(copyRecScoresFromNative(nativeResultContext));
            setClsScores(copyClsScoresFromNative(nativeResultContext));
            setClsLabels(copyClsLabelsFromNative(nativeResultContext));
        }
        // WARN: must release ctx.
        return releaseNative(nativeResultContext);
    }

    private void setBoxes(@NonNull int[] boxesBuffer) {
        int boxesNum = boxesBuffer.length / 8;
        if (boxesNum > 0) {
            mBoxes = new int[boxesNum][8];
            for (int i = 0; i < boxesNum; ++i) {
                mBoxes[i] = Arrays.copyOfRange(
                        boxesBuffer, i * 8, (i + 1) * 8);
            }
        }
    }

    private void setText(@NonNull String[] textBuffer) {
        if (textBuffer.length > 0) {
            mText = textBuffer.clone();
        }
    }

    private void setRecScores(@NonNull float[] recScoresBuffer) {
        if (recScoresBuffer.length > 0) {
            mRecScores = recScoresBuffer.clone();
        }
    }

    private void setClsScores(@NonNull float[] clsScoresBuffer) {
        if (clsScoresBuffer.length > 0) {
            mClsScores = clsScoresBuffer.clone();
        }
    }

    private void setClsLabels(@NonNull int[] clsLabelBuffer) {
        if (clsLabelBuffer.length > 0) {
            mClsLabels = clsLabelBuffer.clone();
        }
    }

    // Fetch native buffers from native context.
    private native int copyBoxesNumFromNative(long nativeResultContext);

    private native int[] copyBoxesFromNative(long nativeResultContext);

    private native String[] copyTextFromNative(long nativeResultContext);

    private native float[] copyRecScoresFromNative(long nativeResultContext);

    private native float[] copyClsScoresFromNative(long nativeResultContext);

    private native int[] copyClsLabelsFromNative(long nativeResultContext);

    private native boolean releaseNative(long nativeResultContext);

    // Initializes at the beginning.
    static {
        FastDeployInitializer.init();
    }
}

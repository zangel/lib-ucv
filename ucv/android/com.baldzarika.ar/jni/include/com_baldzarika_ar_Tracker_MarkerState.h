/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_baldzarika_ar_Tracker_MarkerState */

#ifndef _Included_com_baldzarika_ar_Tracker_MarkerState
#define _Included_com_baldzarika_ar_Tracker_MarkerState
#ifdef __cplusplus
extern "C" {
#endif
#undef com_baldzarika_ar_Tracker_MarkerState_SC_DETECTION
#define com_baldzarika_ar_Tracker_MarkerState_SC_DETECTION 0L
#undef com_baldzarika_ar_Tracker_MarkerState_SC_POSE
#define com_baldzarika_ar_Tracker_MarkerState_SC_POSE 1L
#undef com_baldzarika_ar_Tracker_MarkerState_SC_DETECT_NOTIFY
#define com_baldzarika_ar_Tracker_MarkerState_SC_DETECT_NOTIFY 2L
/*
 * Class:     com_baldzarika_ar_Tracker_MarkerState
 * Method:    initialize
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_baldzarika_ar_Tracker_00024MarkerState_initialize
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_baldzarika_ar_Tracker_MarkerState
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_baldzarika_ar_Tracker_00024MarkerState_destroy
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_Tracker_MarkerState
 * Method:    getTracker
 * Signature: ()Lcom/baldzarika/ar/Tracker;
 */
JNIEXPORT jobject JNICALL Java_com_baldzarika_ar_Tracker_00024MarkerState_getTracker
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_Tracker_MarkerState
 * Method:    isDetected
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_baldzarika_ar_Tracker_00024MarkerState_isDetected
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_Tracker_MarkerState
 * Method:    getMarkerSize
 * Signature: ()Lcom/baldzarika/ar/Size2;
 */
JNIEXPORT jobject JNICALL Java_com_baldzarika_ar_Tracker_00024MarkerState_getMarkerSize
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_Tracker_MarkerState
 * Method:    getHomography
 * Signature: ()Landroid/graphics/Matrix;
 */
JNIEXPORT jobject JNICALL Java_com_baldzarika_ar_Tracker_00024MarkerState_getHomography
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_Tracker_MarkerState
 * Method:    getCameraPose
 * Signature: ([F)Z
 */
JNIEXPORT jboolean JNICALL Java_com_baldzarika_ar_Tracker_00024MarkerState_getCameraPose
  (JNIEnv *, jobject, jfloatArray);

#ifdef __cplusplus
}
#endif
#endif

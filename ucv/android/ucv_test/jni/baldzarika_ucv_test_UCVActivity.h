/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class baldzarika_ucv_test_UCVActivity */

#ifndef _Included_baldzarika_ucv_test_UCVActivity
#define _Included_baldzarika_ucv_test_UCVActivity
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    initialize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_baldzarika_ucv_test_UCVActivity_initialize
  (JNIEnv *, jobject);

/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    terminate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_baldzarika_ucv_test_UCVActivity_terminate
  (JNIEnv *, jobject);

/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    onSurfaceCreated
 * Signature: (Ljavax/microedition/khronos/opengles/GL10;Ljavax/microedition/khronos/egl/EGLConfig;)V
 */
JNIEXPORT void JNICALL Java_baldzarika_ucv_test_UCVActivity_onSurfaceCreated
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    onSurfaceChanged
 * Signature: (Ljavax/microedition/khronos/opengles/GL10;II)V
 */
JNIEXPORT void JNICALL Java_baldzarika_ucv_test_UCVActivity_onSurfaceChanged
  (JNIEnv *, jobject, jobject, jint, jint);

/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    onTouchEvent
 * Signature: (Landroid/view/MotionEvent;)Z
 */
JNIEXPORT jboolean JNICALL Java_baldzarika_ucv_test_UCVActivity_onTouchEvent
  (JNIEnv *, jobject, jobject);

/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    onDrawFrame
 * Signature: (Ljavax/microedition/khronos/opengles/GL10;)V
 */
JNIEXPORT void JNICALL Java_baldzarika_ucv_test_UCVActivity_onDrawFrame
  (JNIEnv *, jobject, jobject);

/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    surfaceChanged
 * Signature: (Landroid/view/SurfaceHolder;III)V
 */
JNIEXPORT void JNICALL Java_baldzarika_ucv_test_UCVActivity_surfaceChanged
  (JNIEnv *, jobject, jobject, jint, jint, jint);

/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    surfaceCreated
 * Signature: (Landroid/view/SurfaceHolder;)V
 */
JNIEXPORT void JNICALL Java_baldzarika_ucv_test_UCVActivity_surfaceCreated
  (JNIEnv *, jobject, jobject);

/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    surfaceDestroyed
 * Signature: (Landroid/view/SurfaceHolder;)V
 */
JNIEXPORT void JNICALL Java_baldzarika_ucv_test_UCVActivity_surfaceDestroyed
  (JNIEnv *, jobject, jobject);

/*
 * Class:     baldzarika_ucv_test_UCVActivity
 * Method:    onPreviewFrame
 * Signature: ([BLandroid/hardware/Camera;)V
 */
JNIEXPORT void JNICALL Java_baldzarika_ucv_test_UCVActivity_onPreviewFrame
  (JNIEnv *, jobject, jbyteArray, jobject);

#ifdef __cplusplus
}
#endif
#endif

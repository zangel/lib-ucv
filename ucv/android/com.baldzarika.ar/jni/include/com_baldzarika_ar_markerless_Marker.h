/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_baldzarika_ar_markerless_Marker */

#ifndef _Included_com_baldzarika_ar_markerless_Marker
#define _Included_com_baldzarika_ar_markerless_Marker
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_baldzarika_ar_markerless_Marker
 * Method:    initialize
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_baldzarika_ar_markerless_Marker_initialize
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_baldzarika_ar_markerless_Marker
 * Method:    create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_baldzarika_ar_markerless_Marker_create__
  (JNIEnv *, jclass);

/*
 * Class:     com_baldzarika_ar_markerless_Marker
 * Method:    create
 * Signature: (Lcom/baldzarika/ar/Size2;)J
 */
JNIEXPORT jlong JNICALL Java_com_baldzarika_ar_markerless_Marker_create__Lcom_baldzarika_ar_Size2_2
  (JNIEnv *, jclass, jobject);

/*
 * Class:     com_baldzarika_ar_markerless_Marker
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_baldzarika_ar_markerless_Marker_destroy
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_markerless_Marker
 * Method:    getSize
 * Signature: ()Lcom/baldzarika/ar/Size2;
 */
JNIEXPORT jobject JNICALL Java_com_baldzarika_ar_markerless_Marker_getSize
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_markerless_Marker
 * Method:    loadImage
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_baldzarika_ar_markerless_Marker_loadImage
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_baldzarika_ar_markerless_Marker
 * Method:    setImage
 * Signature: (Landroid/graphics/Bitmap;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_baldzarika_ar_markerless_Marker_setImage
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_baldzarika_ar_markerless_Marker
 * Method:    saveImage
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_baldzarika_ar_markerless_Marker_saveImage
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif

/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_baldzarika_ar_Marker */

#ifndef _Included_com_baldzarika_ar_Marker
#define _Included_com_baldzarika_ar_Marker
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_baldzarika_ar_Marker
 * Method:    create
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_baldzarika_ar_Marker_create__
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_Marker
 * Method:    create
 * Signature: (Lcom/baldzarika/ar/Size2;)V
 */
JNIEXPORT void JNICALL Java_com_baldzarika_ar_Marker_create__Lcom_baldzarika_ar_Size2_2
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_baldzarika_ar_Marker
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_baldzarika_ar_Marker_destroy
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_Marker
 * Method:    getSize
 * Signature: ()Lcom/baldzarika/ar/Size2;
 */
JNIEXPORT jobject JNICALL Java_com_baldzarika_ar_Marker_getSize
  (JNIEnv *, jobject);

/*
 * Class:     com_baldzarika_ar_Marker
 * Method:    loadImage
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_baldzarika_ar_Marker_loadImage
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_baldzarika_ar_Marker
 * Method:    setImage
 * Signature: (Landroid/graphics/Bitmap;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_baldzarika_ar_Marker_setImage
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_baldzarika_ar_Marker
 * Method:    saveImage
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_baldzarika_ar_Marker_saveImage
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif

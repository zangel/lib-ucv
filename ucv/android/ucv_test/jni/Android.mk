LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#define OPENCV_INCLUDES
include $(OPENCV_ROOT)/android/build/android-opencv.mk
#define OPENCV_LIBS
#include $(OPENCV_ROOT)/android/build/libs.mk


LOCAL_MODULE    := ucv_test
### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES :=	ucv_test.cpp \
					UCVActivity.cpp \
					../../../lib_ucv/src/baldzarika/ucv/config.cpp \
					../../../lib_ucv/src/baldzarika/ucv/feature_point.cpp \
					../../../lib_ucv/src/baldzarika/ucv/surf.cpp
					
					

LOCAL_C_INCLUDES	:=	$(ANDROID_CPP_SDK)/j2cpp \
						$(ANDROID_CPP_SDK)/platforms/android-7 \
						$(ANDROID_BOOST_DIR) \
						$(LOCAL_PATH)/../../../lib_ucv/include \
						$(UCV_DIR)/include \
						$(OPENCV_INCLUDES)
						

LOCAL_LDLIBS    	:=	-L$(ANDROID_BOOST_DIR)/android/lib \
						-lboost_thread \
						-lboost_system \
						-L$(OPENCV_ROOT)/android/build/obj/local/armeabi \
						-lopencv_features2d \
						-lopencv_objdetect \
						-lopencv_highgui \
						-lopencv_video \
						-lopencv_flann \
						-lopencv_calib3d \
						-lopencv_imgproc \
						-lopencv_core \
						-lopencv_lapack \
						-lzlib \
						-lpng \
						-ljpeg \
						-ljasper \
						-lGLESv2 \
						-ldl \
						-llog

include $(BUILD_SHARED_LIBRARY)

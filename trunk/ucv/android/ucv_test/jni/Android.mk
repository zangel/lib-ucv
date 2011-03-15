LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := ucv_test
### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES :=	ucv_test.cpp \
					UCVActivity.cpp \
					../../../lib_ucv/src/baldzarika/ucv/config.cpp \
					../../../lib_ucv/src/baldzarika/ucv/feature_point.cpp \
					../../../lib_ucv/src/baldzarika/ucv/surf.cpp \
					../../../lib_ucv/src/baldzarika/ucv/gles/shaders.cpp
					
					

LOCAL_C_INCLUDES	:=	$(ANDROID_CPP_SDK)/j2cpp \
						$(ANDROID_CPP_SDK)/platforms/android-7 \
						$(ANDROID_BOOST_DIR) \
						$(LOCAL_PATH)/../../../lib_ucv/include \
						$(UCV_DIR)/include

						

LOCAL_LDLIBS    	:=	-L$(ANDROID_BOOST_DIR)/android/lib \
						-lGLESv2 \
						-ldl \
						-llog

include $(BUILD_SHARED_LIBRARY)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := com.baldzarika.ar

LOCAL_C_INCLUDES	:=	$(ANDROID_CPP_SDK)/j2cpp \
						$(ANDROID_CPP_SDK)/platforms/android-8 \
						$(ANDROID_BOOST_DIR) \
						$(OPENCV_ROOT)/3rdparty/libpng \
						$(OPENCV_ROOT)/3rdparty/include \
						$(LOCAL_PATH)/../../../3rd_party/libkdtree \
						$(LOCAL_PATH)/../../../lib_ucv/include \
						$(LOCAL_PATH)/../../../lib_ar/include \
						$(LOCAL_PATH)/include

LOCAL_SRC_FILES :=	src/support.cpp \
					src/com/baldzarika/ar/Size2.cpp \
					src/com/baldzarika/ar/Frame.cpp \
					src/com/baldzarika/ar/Marker.cpp \
					../../../lib_ar/src/baldzarika/ar/marker.cpp
					
					
LOCAL_LDLIBS    	:=	-L$(ANDROID_BOOST_DIR)/android/lib \
						-L$(OPENCV_ROOT)/android/build/obj/local/armeabi \
						-lboost_system \
						-lpng \
						-lzlib \
						-ldl \
						-llog

include $(BUILD_SHARED_LIBRARY)

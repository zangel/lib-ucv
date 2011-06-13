LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := com.baldzarika.ar

LOCAL_C_INCLUDES	:=	$(ANDROID_CPP_SDK)/j2cpp \
						$(ANDROID_CPP_SDK)/platforms/android-7 \
						$(ANDROID_BOOST_DIR) \
						$(OPENCV_ROOT)/3rdparty/libpng \
						$(OPENCV_ROOT)/3rdparty/include \
						$(LOCAL_PATH)/../../../3rd_party/libkdtree \
						$(LOCAL_PATH)/../../../lib_math/include \
						$(LOCAL_PATH)/../../../lib_ucv/include \
						$(LOCAL_PATH)/../../../lib_ar/include \
						$(LOCAL_PATH)/include

LOCAL_SRC_FILES :=	src/support.cpp \
					src/com/baldzarika/ar/Size2.cpp \
					src/com/baldzarika/ar/Point2.cpp \
					src/com/baldzarika/ar/Tracker.cpp \
					src/com/baldzarika/ar/fiducial/Tracker.cpp \
					src/com/baldzarika/ar/fiducial/MarkerModel.cpp \
					src/com/baldzarika/ar/fiducial/BCHMarkerModel.cpp \
					../../../lib_ar/src/baldzarika/ar/tracker.cpp \
					../../../lib_ar/src/baldzarika/ar/markerless/marker.cpp \
					../../../lib_ar/src/baldzarika/ar/markerless/tracker.cpp \
					../../../lib_ar/src/baldzarika/ar/fiducial/marker_model.cpp \
					../../../lib_ar/src/baldzarika/ar/fiducial/bch_marker_model.cpp \
					../../../lib_ar/src/baldzarika/ar/fiducial/tracker.cpp \
					../../../lib_ucv/src/baldzarika/ucv/surf.cpp \
					../../../lib_ucv/src/baldzarika/ucv/homography.cpp
					
					
					
					
LOCAL_LDLIBS    	:=	-L$(ANDROID_BOOST_DIR)/android/lib \
						-L$(OPENCV_ROOT)/android/build/obj/local/armeabi \
						-lboost_system \
						-l boost_thread \
						-lpng \
						-lzlib \
						-ldl \
						-llog

include $(BUILD_SHARED_LIBRARY)

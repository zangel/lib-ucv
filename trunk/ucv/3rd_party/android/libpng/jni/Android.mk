LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := png

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_ARM_NEON := true
endif

LOCAL_SRC_FILES :=   png.c pngerror.c pngget.c pngmem.c pngpread.c pngread.c pngrio.c pngrtran.c pngrutil.c pngset.c pngtest.c pngtrans.c pngwio.c pngwrite.c pngwtran.c pngwutil.c

LOCAL_CFLAGS := 

include $(BUILD_STATIC_LIBRARY)

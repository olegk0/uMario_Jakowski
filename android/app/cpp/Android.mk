LOCAL_PATH := $(call my-dir)

# $(info $$var is [${var}])

#APP_STL := c++_static

include $(CLEAR_VARS)
LOCAL_MODULE := mpg123
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/$(APP_ABI)/libmpg123.so
#LOCAL_EXPORT_C_INCLUDES :=
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := hidapi
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/$(APP_ABI)/libhidapi.so
#LOCAL_EXPORT_C_INCLUDES :=
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_image
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/$(APP_ABI)/libSDL2_image.so
#LOCAL_EXPORT_C_INCLUDES :=
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_mixer
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/$(APP_ABI)/libSDL2_mixer.so
#LOCAL_EXPORT_C_INCLUDES :=
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/$(APP_ABI)/libSDL2.so
#LOCAL_EXPORT_C_INCLUDES :=
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
# DO NOT CHANGE "main" - libmain.so  object is "Executable"
LOCAL_MODULE := main

#LOCAL_CFLAGS :=
#LOCAL_LDFLAGS := 
#LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -Wl,--rpath=.
LOCAL_LDLIBS :=  -lGLESv2
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
       /home/iam/AndroidStudioProjects/games/SDL2-Android/build/SDL2-2.0.12/include \
       /home/iam/AndroidStudioProjects/games/SDL2-Android/build/SDL2_image-2.0.5 \
       /home/iam/AndroidStudioProjects/games/SDL2-Android/build/SDL2_mixer-2.0.4

# Add your <name>.c files here
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/*.cpp)

LOCAL_SHARED_LIBRARIES := hidapi mpg123 SDL2 SDL2_image SDL2_mixer
include $(BUILD_SHARED_LIBRARY)

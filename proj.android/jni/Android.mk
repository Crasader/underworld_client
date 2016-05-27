LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libevent_static
LOCAL_SRC_FILES := third/libevent2.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

define all-cpp-files
$(patsubst jni/%,%, $(shell find $(LOCAL_PATH)/../../Classes $(LOCAL_PATH)/underworld -name "*.cpp" -or -name "*.c"))
endef

LOCAL_SRC_FILES := $(call all-cpp-files)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)

FILE_INCLUDES := $(shell find $(LOCAL_PATH)/../../Classes -type d)
LOCAL_C_INCLUDES := $(FILE_INCLUDES) \
    $(LOCAL_PATH)/third/libeventh \
	$(LOCAL_PATH)/third/libeventh/include \
	$(LOCAL_PATH)/third/libeventh/android

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static
LOCAL_STATIC_LIBRARIES += libevent_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

LOCAL_CFLAGS := -DHAVE_CONFIG_H -DANDROID -fvisibility=hidden

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END

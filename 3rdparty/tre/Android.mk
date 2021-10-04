LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tre

common_C_INCLUDES := $(LOCAL_PATH) \
					 $(LOCAL_PATH)/lib \
					 $(LOCAL_PATH)/lib/tre

common_SRC_FILES := lib/tre-ast.c \
					lib/tre-compile.c \
					lib/tre-match-backtrack.c \
					lib/tre-match-parallel.c \
					lib/tre-mem.c \
					lib/tre-parse.c \
					lib/tre-stack.c \
					lib/regcomp.c \
					lib/regexec.c \
					lib/regerror.c \
					lib/tre-match-approx.c

common_CFLAGS := -fPIC -ffast-math -DHAVE_CONFIG_H

# For the host
# =====================================================

LOCAL_SRC_FILES := $(common_SRC_FILES)
LOCAL_CFLAGS += -O3 $(common_CFLAGS)
LOCAL_C_INCLUDES += $(common_C_INCLUDES)

LOCAL_LDLIBS :=

include $(BUILD_STATIC_LIBRARY)
# include $(BUILD_SHARED_LIBRARY)

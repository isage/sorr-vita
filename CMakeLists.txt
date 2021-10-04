cmake_minimum_required(VERSION 3.4)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

project(sorr C)

add_definitions("-Dmemcpy=sceClibMemcpy")
add_definitions("-Dmemcmp=sceClibMemcmp")
add_definitions("-Dstrcmp=sceClibStrcmp")
add_definitions("-Dstrncmp=sceClibStrncmp")
add_definitions("-Dstrncpy=sceClibStrncpy")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast")
#add_definitions("-ffast-math -ftree-vectorize -mfpu=neon")

# Require tre
add_subdirectory(3rdparty/tre)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(SDL2 2.0.4 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(OGG REQUIRED)
find_package(Vorbis REQUIRED)

# Preprocessor macros
include(common_defs)

set(BGDRTM_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/copy.c
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/dcbr.c
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/dirs.c
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/instance.c
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/interpreter.c
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/misc.c
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/strings.c
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/sysprocs.c
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/varspace_file.c
  ${CMAKE_SOURCE_DIR}/core/bgdrtm/src/fmath.c
  ${CMAKE_SOURCE_DIR}/core/common/debug.c
  ${CMAKE_SOURCE_DIR}/core/common/files.c
  ${CMAKE_SOURCE_DIR}/core/common/xctype.c
  ${CMAKE_SOURCE_DIR}/modules/libbgload/bgload.c
  ${CMAKE_SOURCE_DIR}/modules/mod_say/mod_say.c
  ${CMAKE_SOURCE_DIR}/modules/mod_string/mod_string.c
  ${CMAKE_SOURCE_DIR}/modules/mod_math/mod_math.c
  ${CMAKE_SOURCE_DIR}/modules/mod_mem/mod_mem.c
  ${CMAKE_SOURCE_DIR}/modules/mod_time/mod_time.c
  ${CMAKE_SOURCE_DIR}/modules/mod_file/mod_file.c
  ${CMAKE_SOURCE_DIR}/modules/mod_sound/mod_sound.c
  ${CMAKE_SOURCE_DIR}/modules/libsdlhandler/libsdlhandler.c
  ${CMAKE_SOURCE_DIR}/modules/libjoy/libjoy.c
  ${CMAKE_SOURCE_DIR}/modules/mod_joy/mod_joy.c
  ${CMAKE_SOURCE_DIR}/modules/libkey/libkey.c
  ${CMAKE_SOURCE_DIR}/modules/mod_key/mod_key.c
  ${CMAKE_SOURCE_DIR}/modules/mod_proc/mod_proc.c
  ${CMAKE_SOURCE_DIR}/modules/mod_sort/mod_sort.c
  ${CMAKE_SOURCE_DIR}/modules/mod_timers/mod_timers.c
  ${CMAKE_SOURCE_DIR}/modules/libgrbase/g_bitmap.c
  ${CMAKE_SOURCE_DIR}/modules/libgrbase/g_clear.c
  ${CMAKE_SOURCE_DIR}/modules/libgrbase/g_grlib.c
  ${CMAKE_SOURCE_DIR}/modules/libgrbase/g_pal.c
  ${CMAKE_SOURCE_DIR}/modules/libgrbase/g_blendop.c
  ${CMAKE_SOURCE_DIR}/modules/libgrbase/g_conversion.c
  ${CMAKE_SOURCE_DIR}/modules/libgrbase/libgrbase.c
  ${CMAKE_SOURCE_DIR}/modules/libblit/g_blit.c
  ${CMAKE_SOURCE_DIR}/modules/libblit/g_pixel.c
  ${CMAKE_SOURCE_DIR}/modules/libblit/libblit.c
  ${CMAKE_SOURCE_DIR}/modules/libvideo/g_regions.c
  ${CMAKE_SOURCE_DIR}/modules/libvideo/g_video.c
  ${CMAKE_SOURCE_DIR}/modules/librender/g_fade.c
  ${CMAKE_SOURCE_DIR}/modules/librender/g_frame.c
  ${CMAKE_SOURCE_DIR}/modules/librender/g_instance.c
  ${CMAKE_SOURCE_DIR}/modules/librender/g_object.c
  ${CMAKE_SOURCE_DIR}/modules/librender/g_rects.c
  ${CMAKE_SOURCE_DIR}/modules/librender/g_screen.c
  ${CMAKE_SOURCE_DIR}/modules/librender/librender.c
  ${CMAKE_SOURCE_DIR}/modules/mod_video/mod_video.c
  ${CMAKE_SOURCE_DIR}/modules/libmouse/libmouse.c
  ${CMAKE_SOURCE_DIR}/modules/mod_multi/mod_multi.c
  ${CMAKE_SOURCE_DIR}/modules/mod_map/file_fnt.c
  ${CMAKE_SOURCE_DIR}/modules/mod_map/file_fpg.c
  ${CMAKE_SOURCE_DIR}/modules/mod_map/file_map.c
  ${CMAKE_SOURCE_DIR}/modules/mod_map/file_pal.c
  ${CMAKE_SOURCE_DIR}/modules/mod_map/file_pcx.c
  ${CMAKE_SOURCE_DIR}/modules/mod_map/file_png.c
  ${CMAKE_SOURCE_DIR}/modules/mod_map/mod_map.c
  ${CMAKE_SOURCE_DIR}/modules/libfont/libfont.c
  ${CMAKE_SOURCE_DIR}/modules/mod_dir/mod_dir.c
  ${CMAKE_SOURCE_DIR}/modules/libtext/libtext.c
  ${CMAKE_SOURCE_DIR}/modules/mod_text/mod_text.c
  ${CMAKE_SOURCE_DIR}/modules/mod_rand/mod_rand.c
  ${CMAKE_SOURCE_DIR}/modules/mod_grproc/mod_grproc.c
  ${CMAKE_SOURCE_DIR}/modules/libscroll/libscroll.c
  ${CMAKE_SOURCE_DIR}/modules/mod_scroll/mod_scroll.c
  ${CMAKE_SOURCE_DIR}/modules/libdraw/libdraw.c
  ${CMAKE_SOURCE_DIR}/modules/mod_draw/mod_draw.c
  ${CMAKE_SOURCE_DIR}/modules/mod_regex/mod_regex.c
  ${CMAKE_SOURCE_DIR}/modules/mod_screen/mod_screen.c
  ${CMAKE_SOURCE_DIR}/modules/mod_path/mod_path.c
  ${CMAKE_SOURCE_DIR}/modules/mod_effects/mod_effects.c
  ${CMAKE_SOURCE_DIR}/modules/mod_blendop/mod_blendop.c
  ${CMAKE_SOURCE_DIR}/modules/mod_m7/mod_m7.c
  ${CMAKE_SOURCE_DIR}/modules/libwm/libwm.c
  ${CMAKE_SOURCE_DIR}/modules/mod_wm/mod_wm.c
  ${CMAKE_SOURCE_DIR}/modules/mod_sys/mod_sys.c
  ${CMAKE_SOURCE_DIR}/modules/mod_flic/mod_flic.c)


add_library(bgdrtm STATIC ${BGDRTM_SOURCE_FILES})
target_compile_definitions(bgdrtm PUBLIC -D__BGDRTM__)

target_include_directories(bgdrtm PRIVATE
                    ${CMAKE_SOURCE_DIR}/core/bgdrtm/include
                    ${CMAKE_SOURCE_DIR}/core/include
                    ${CMAKE_SOURCE_DIR}/modules/libbgload/
                    ${CMAKE_SOURCE_DIR}/modules/mod_say/
                    ${CMAKE_SOURCE_DIR}/modules/mod_string/
                    ${CMAKE_SOURCE_DIR}/modules/mod_math/
                    ${CMAKE_SOURCE_DIR}/modules/mod_mem/
                    ${CMAKE_SOURCE_DIR}/modules/mod_time/
                    ${CMAKE_SOURCE_DIR}/modules/mod_file/
                    ${CMAKE_SOURCE_DIR}/modules/mod_sound/
                    ${CMAKE_SOURCE_DIR}/modules/libsdlhandler/
                    ${CMAKE_SOURCE_DIR}/modules/mod_joy/
                    ${CMAKE_SOURCE_DIR}/modules/libjoy/
                    ${CMAKE_SOURCE_DIR}/modules/libkey/
                    ${CMAKE_SOURCE_DIR}/modules/mod_key/
                    ${CMAKE_SOURCE_DIR}/modules/mod_proc/
                    ${CMAKE_SOURCE_DIR}/modules/mod_sort/
                    ${CMAKE_SOURCE_DIR}/modules/mod_timers/
                    ${CMAKE_SOURCE_DIR}/modules/libgrbase/
                    ${CMAKE_SOURCE_DIR}/modules/libblit/
                    ${CMAKE_SOURCE_DIR}/modules/libvideo/
                    ${CMAKE_SOURCE_DIR}/modules/librender/
                    ${CMAKE_SOURCE_DIR}/modules/mod_video/
                    ${CMAKE_SOURCE_DIR}/modules/libmouse/
                    ${CMAKE_SOURCE_DIR}/modules/mod_mouse/
                    ${CMAKE_SOURCE_DIR}/modules/mod_map/
                    ${CMAKE_SOURCE_DIR}/modules/libfont/
                    ${CMAKE_SOURCE_DIR}/modules/mod_dir/
                    ${CMAKE_SOURCE_DIR}/modules/libtext/
                    ${CMAKE_SOURCE_DIR}/modules/mod_text/
                    ${CMAKE_SOURCE_DIR}/modules/mod_rand/
                    ${CMAKE_SOURCE_DIR}/modules/mod_grproc/
                    ${CMAKE_SOURCE_DIR}/modules/libscroll/
                    ${CMAKE_SOURCE_DIR}/modules/mod_scroll/
                    ${CMAKE_SOURCE_DIR}/modules/libdraw/
                    ${CMAKE_SOURCE_DIR}/modules/mod_draw/
                    ${CMAKE_SOURCE_DIR}/modules/mod_screen/
                    ${CMAKE_SOURCE_DIR}/modules/mod_path/
                    ${CMAKE_SOURCE_DIR}/modules/mod_effects/
                    ${CMAKE_SOURCE_DIR}/modules/mod_blendop/
                    ${CMAKE_SOURCE_DIR}/modules/mod_m7/
                    ${CMAKE_SOURCE_DIR}/modules/libwm/
                    ${CMAKE_SOURCE_DIR}/modules/mod_wm/
                    ${CMAKE_SOURCE_DIR}/modules/mod_sys/
                    ${CMAKE_SOURCE_DIR}/modules/mod_flic/
                    ${CMAKE_SOURCE_DIR}/modules/mod_regex/
                    ${CMAKE_SOURCE_DIR}/modules/mod_multi/
                    ${CMAKE_SOURCE_DIR}/modules/mod_chipmunk/
                    ${CMAKE_SOURCE_DIR}/modules/mod_iap/

                    ${SDL2_INCLUDE_DIR}
                    ${SDL2_MIXER_INCLUDE_DIRS}
                    ${ZLIB_INCLUDE_DIRS}
                    ${VORBIS_INCLUDE_DIR}
                    ${TRE_INCLUDE_DIR}
                    ${PNG_INCLUDE_DIR})


# Source files
set(BGDI_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/core/bgdi/src/main.c
)

# Include dirs

set(EXTRA_LIBS "-lm")

# Main executable
add_executable(${PROJECT_NAME} ${BGDI_SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__BGDI__)
target_include_directories(${PROJECT_NAME} PRIVATE
                    ${CMAKE_SOURCE_DIR}/core/bgdi/include
                    ${CMAKE_SOURCE_DIR}/core/include
                    ${CMAKE_SOURCE_DIR}/core/bgdrtm/include/)

# Include the compiler flags from the external include file
include(compiler_flags)

# Link the executable to the libraries
target_link_libraries(${PROJECT_NAME}
                      bgdrtm
                      ${SDL2_MIXER_LIBRARIES}
                      SDL2::SDL2-static
                      ${PNG_LIBRARIES}
                      libtre
                      ${ZLIB_LIBRARIES}
                      FLAC
                      vorbisfile
                      ${VORBIS_LIBRARY}
                      ${OGG_LIBRARY}
                      mikmod
                      mpg123
                      ${EXTRA_LIBS})

set(VITA_APP_NAME "SoRR")
set(VITA_TITLEID  "SORR00001")
set(VITA_VERSION  "01.00")
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE2=12")

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} UNSAFE)
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE sce_sys sce_sys
)

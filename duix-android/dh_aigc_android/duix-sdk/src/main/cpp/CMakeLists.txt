project(scrfdncnn)

cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -funwind-tables")

set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/third/opencv-mobile-4.6.0-android/sdk/native/jni)
find_package(OpenCV REQUIRED core imgproc highgui)

#set(ncnn_DIR ${CMAKE_SOURCE_DIR}/third/ncnn-20221128-android-vulkan-shared/${ANDROID_ABI}/lib/cmake/ncnn)
set(ncnn_DIR ${CMAKE_SOURCE_DIR}/third/ncnn-20231027-android-shared/${ANDROID_ABI}/lib/cmake/ncnn)
find_package(ncnn REQUIRED)


option(USE_OPENCV "shared library support" TRUE)
option(USE_NCNN "shared library support" TRUE)

include_directories(
    include
    base
    render
    aisdk
    aes
    digit
    android
    third/arm/include
    third/arm/include/turbojpeg
    )

add_library(scrfdncnn SHARED
    android/DigitJni.cpp
    android/MsgcbJni.cpp
    android/JniHelper.cpp
    aisdk/jmat.cpp
    aisdk/wavreader.cpp
    aisdk/wenet.cpp
    aisdk/aimodel.cpp
    aisdk/scrfd.cpp
    aisdk/pfpld.cpp
    aisdk/munet.cpp
    aisdk/malpha.cpp
    aisdk/wavcache.cpp
    aisdk/blendgram.cpp
    aisdk/face_utils.cpp
    aisdk/netwav.cpp
    digit/looper.cpp
    digit/netcurl.cpp
    digit/GRender.cpp
    digit/GDigit.cpp
    digit/dispatchqueue.cpp
    render/EglRenderer.cpp
    render/RgbVideoRenderer.cpp
    render/SurfaceVideoRenderer.cpp
    render/RenderHelper.cpp
    base/BaseRenderHelper.cpp
    base/AudioTrack.cpp
    render/AudioRenderer.cpp
    render/GlesProgram.cpp
    base/Log.cpp
    base/FrameSource.cpp
    base/MediaData.cpp
    base/MessageSource.cpp
    base/MessageHelper.cpp
    base/LoopThread.cpp
    base/XThread.cpp
    base/XTick.c
    base/cJSON.c
    base/dh_mem.c
    digit/grtcfg.c
    base/LoopThreadHelper.cpp
    base/coffeecatch.c
    aes/aes_cbc.c  aes/aes_core.c  aes/aes_ecb.c  aes/base64.c  aes/cbc128.c  aes/gj_aes.c
    aes/aesmain.c
    )


add_library(turbojpeg STATIC IMPORTED)
set_target_properties(turbojpeg
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/libturbojpeg.a)

add_library(libjpeg STATIC IMPORTED)
set_target_properties(libjpeg
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/libjpeg.a)

    #add_library(pplcommon STATIC IMPORTED)
#set_target_properties(pplcommon
    #        PROPERTIES IMPORTED_LOCATION
        #        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/libpplcommon_static.a)

    #add_library(pplcv STATIC IMPORTED)
#set_target_properties(pplcv
    #        PROPERTIES IMPORTED_LOCATION
        #        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/libpplcv_static.a)

add_library(curl STATIC IMPORTED)
set_target_properties(curl
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/libcurl.a)

add_library(ssl STATIC IMPORTED)
set_target_properties(ssl
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/libssl.a)

add_library(crypto STATIC IMPORTED)
set_target_properties(crypto
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/libcrypto.a)

add_library(avcodec STATIC IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/ffmpeg-lite/libavcodec.a)

add_library(avformat STATIC IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/ffmpeg-lite/libavformat.a)

add_library(avutil STATIC IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/ffmpeg-lite/libavutil.a)

add_library(swresample STATIC IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/ffmpeg-lite/libswresample.a)

add_library(swscale STATIC IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/ffmpeg-lite/libswscale.a)

find_library(log-lib log)

add_library(onnx-lib SHARED IMPORTED)
set_target_properties(
        onnx-lib
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/third/arm/${ANDROID_ABI}/libonnxruntime.so)

target_link_libraries(scrfdncnn
    ncnn
    ${OpenCV_LIBS}
    ${log-lib}
    onnx-lib
    camera2ndk
    mediandk
    libjpeg
    turbojpeg
    avformat
    avcodec
    avutil
    swresample
    swscale
    curl
    ssl
    crypto
    -landroid
    -lmediandk
    -lEGL
    -lGLESv2
    -lm -lz
)


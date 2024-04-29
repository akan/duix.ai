ANDROID_NDK=~/tools/android-ndk-r25c
TOOLCHAIN=$ANDROID_NDK/build/cmake/android.toolchain.cmake
BUILD_DIR=android-arm64
mkdir -p $BUILD_DIR
cd $BUILD_DIR
#-G Ninja # fail
cmake \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN \
    -DANDROID_LD=lld \
    -DANDROID_ABI="arm64-v8a" \
    -DANDROID_PLATFORM=android-24 \
    -DCMAKE_BUILD_TYPE=Release \
    -DPPLCV_USE_AARCH64=ON \
    ..

# -DHPCC_USE_AARCH64=ON \


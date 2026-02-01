#! /bin/bash

sdk="/var/theos/sdks/iPhoneOS17.5.sdk"
osx="arm64-apple-ios14.0"
build="obj"
cxx="clang++"
common_flags=(
    -std=c++20
    -stdlib=libc++
    -isysroot "$sdk"
    -target "$osx"
    -arch arm64
)
dylib_flags=(
    -fPIC
    -shared
    -D_DARWIN_C_SOURCE
)
librarys=(
    -L"$build/lib"
    -Wl,-rpath,@loader_path/../lib
    -Wl,-rpath,/usr/lib/lamina
    -Wl,-rpath,/lib/lamina
    -Wl,-rpath,/usr/local/lib/lamina
    -Wl,-rpath,/var/jb/usr/lib/lamina
    -Wl,-rpath,/var/jb/lib/lamina
    -Wl,-rpath,/var/jb/usr/local/lib/lamina
)
darwin_framework=(
    -framework CoreFoundation
    -framework CoreServices
)

_clean() {
    if [ -d "$build" ]; then
        rm -rf "$build"
        mkdir -p "$build/lib"
        mkdir -p "$build/bin"
    fi
}

build_lmvm_dylib() {
    runtime_src=$(find runtime -name "*.cpp" -o -name "*.c")
    
    "$cxx" "${common_flags[@]}" \
        "${dylib_flags[@]}" \
        -DLMX_DLL \
        -DLMVM_BUILD \
        -I./include \
        $runtime_src \
        "${darwin_framework[@]}" \
        -Wl,-undefined,dynamic_lookup \
        -install_name @rpath/liblmvm.dylib \
        -o "$build/lib/liblmvm.dylib"
}

build_lmc_dylib() {
    compiler_src=$(find compiler \
        -name "*.cpp" \
        -o -name "*.c")
    "$cxx" "${common_flags[@]}" \
        "${dylib_flags[@]}" \
        -DLMX_DLL \
        -DLMC_BUILD \
        -I./include \
        $compiler_src \
        "${darwin_framework[@]}" \
        -Wl,-undefined,dynamic_lookup \
        -install_name @rpath/liblmc.dylib \
        -o "$build/lib/liblmc.dylib"
}

build_lm() {
    "$cxx" "${common_flags[@]}" \
        -DLMX_DLL \
        -I./include \
        "${librarys[@]}" \
        "${darwin_framework[@]}" \
        tools/lm/main.cpp tools/lm/common/repl.cpp tools/lm/common/file_run.cpp \
        -llmvm -llmc \
        -o "$build/bin/lm"
}

ens() {
    ldid -Sentitlements.plist -M -Hsha256 $build/bin/lm && ldid -Sentitlements.plist -M -Hsha256 $build/lib/lib*.dylib
}

main() {
    if command -v dpkg >/dev/null 2>&1; then
        architecture=$(dpkg --print-architecture 2>/dev/null)
        if [[ "$architecture" != "iphoneos-"* ]]; then
            printf "%s error: 這是給 iOS jailbreak 用的\n你應該去用 cmake 而不是這個\n" "$0" >&2
            exit 1
        fi
    fi

    mkdir -p $build/bin
    mkdir -p $build/lib
    build_lmvm_dylib
    build_lmc_dylib
    build_lm
    printf "%s info: 編譯完成,日誌在 log/ 中" $0;
    ens
}

mkdir -p "log";
main 2>"log/err.log" | tee "log/info.log"
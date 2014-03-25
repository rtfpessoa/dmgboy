# Dot Matrix GameBoy Emulator

## Build Instructions (Mac OSx 10.9.2 & XCode 5.1)

###wxWidgets 3.0.0

####Download
https://sourceforge.net/projects/wxwindows/files/3.0.0/wxWidgets-3.0.0.tar.bz2

####Install

      tar -xvf wxWidgets-3.0.0.tar.bz2
      cd wxWidgets-3.0.0
      ./autogen.sh
      mkdir build-release
      cd build-release
      export ARCHVAL="-arch i386"
      export SDK19LOC="/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk"
      export MACMINVER="10.6"
      ../configure \
          --disable-shared \
          --with-opengl \
          --with-osx_cocoa \
          --with-macosx-version-min=${MACMINVER} \
          --with-macosx-sdk=${SDK19LOC} \
          --prefix="$(pwd)" \
          CFLAGS="${ARCHVAL}" \
          CXXFLAGS="${ARCHVAL}" \
          CPPFLAGS="${ARCHVAL}" \
          LDFLAGS="${ARCHVAL}" \
          OBJCFLAGS="${ARCHVAL}" \
          OBJCXXFLAGS="${ARCHVAL}" && make

###PortAudio v19

####Download
http://portaudio.com/archives/pa_stable_v19_20140130.tgz

####Install

    tar -xvf pa_stable_v19_20140130.tgz
    cd portaudio
    export ARCHVAL="-arch i386"
    export SDK19LOC="/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk"
    export MACMINVER="10.6"
    SDKVER="${ARCHVAL} -arch ppc -isysroot ${SDK19LOC} -mmacosx-version-min=${MACMINVER}"
    CFLAGS="-O2 -g -Wall ${SDKVER}" \
        LDFLAGS=" ${SDKVER}" \
        sudo ./configure && make


## Build Instructions (Ubuntu 14.04)

####Requirements

Install CodeBlocks

####Install

    sudo apt-get install libwxgtk2.8-dev libwxgtk2.8-dbg
    sudo apt-get install libglew-dev libglew1.8
    sudo apt-get install portaudio19-dev
    sudo apt-get install xterm

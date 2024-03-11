# LibreOffice SDK Examples for C++

Here are several simple programs intended to demostrate how to use LibreOffice's SDK API (https://api.libreoffice.org/) in C++. Some of the examples are translated from the official Java examples (https://api.libreoffice.org/examples/examples.html). All development and testing were conducted on the `Ubuntu-22.04-x86_64 platform`.

## Build LibreOffice

The build process described below draws heavily from the [official manual](https://wiki.documentfoundation.org/Development/BuildingOnLinux).

### Build dependencies
```shell
sudo apt install git
sudo apt install build-essential 
sudo apt install autoconf
sudo apt install gperf
sudo apt install libxslt1-dev xsltproc
sudo apt install libxml2-utils
sudo apt install libnss3-dev
sudo apt install libx11-dev libxt-dev libxrandr-dev libxinerama-dev
sudo apt install bison
sudo apt install flex
sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-pulseaudio
sudo apt install libcairo2-dev
```

Note that, since Sept 22 2023, `gcc/g++` must be at least version 12. `sudo apt install gcc-12 g++-12` and then `sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 60 --slave /usr/bin/g++ g++ /usr/bin/g++-12`.

### Clone and building

```shell
git clone https://gerrit.libreoffice.org/core libreoffice
```

```shell
./autogen.sh --enable-dbgutil --enable-debug --enable-sal-log --enable-symbols --enable-optimized=no --disable-runtime-optimizations --with-java=no --enable-python=no --disable-cups --with-system-libxml --disable-librelogo --disable-mariadb-sdbc --disable-postgresql-sdbc --disable-firebird-sdbc --with-system-nss --disable-gtk3 --disable-gtk4 --disable-qt5 --disable-qt6 --with-system-cairo --disable-dbus --enable-avmedia --enable-gstreamer-1-0 --with-system-jpeg --enable-odk --enable-ooenv --with-doxygen=no
```

```shell
/usr/bin/make
```

## Build examples

### Requirements

```shell
sudo apt install cmake
```

In this project, I migrated the build system of core/odk/examples from make-based builds to the equivalent CMake.

```CMake
add_compile_definitions(UNX)
add_compile_definitions(DGCC)
add_compile_definitions(LINUX)
add_compile_definitions(DCPPU_ENV=gcc3)
```

These are CMake instructions that add compile definations to libreoffice-sdk-example's build which guarantee that examples are compiled with SDK accurately. Although I haven’t yet discerned their precise specifics, rest assured that they contribute to the correct compilation process.

```CMake
set(SDK_INCLUDE_DIR  "/usr/lib/libreoffice/sdk/include"  CACHE STRING  "OO_SDK_HOME/include")
set(SDK_LIBRARY_DIR  "/usr/lib/libreoffice/sdk/lib"      CACHE STRING  "OO_SDK_HOME/lib")
set(SDK_BINARY_DIR   "/usr/lib/libreoffice/sdk/bin"      CACHE STRING  "OO_SDK_HOME/bin")
set(SDK_PROGRAM_DIR  "/usr/lib/libreoffice/program"      CACHE STRING  "OO_SDK_URE_LIB_DIR")
set(SDK_LIBRARIES    "uno_sal" "uno_salhelpergcc3" "uno_purpenvhelpergcc3" "uno_cppu" "uno_cppuhelpergcc3")
```

These CMake variables define important paths and libraries needed for working with the LibreOffice SDK in a C++ project. Note that The default installation path for the SDK is /usr/lib/libreoffice/sdk/. However, it is necessary to manually compile LibreOffice and its SDK to ensure the correct functioning of some examples. We will configure our manually compiled LibreOffice SDK directory in the build.sh script.

```CMake
execute_process(COMMAND "${SDK_BINARY_DIR}/cppumaker" -Gc -O${PROJECT_SOURCE_DIR}/include "${SDK_PROGRAM_DIR}/types.rdb" "${SDK_PROGRAM_DIR}/types/offapi.rdb")
```
This executes a cppumaker tool during the build process, which generates a C++ representation for IDL types. The `-Gc` flag indicates that only target files with modified content will be generated. The `-O` flag specifies the output directory, and `${PROJECT_SOURCE_DIR}/include` is the destination directory.  `{SDK_PROGRAM_DIR}/types.rdb` and `${SDK_PROGRAM_DIR}/types/offapi.rdb` are the type libraries that cppumaker operates on.

Furthermore, the compilation and linking process requires some additional parameters, whose functions will be explained as needed.

### Clone and building

```shell
git clone https://github.com/aniuzhong/libreoffice-sdk-examples.git
cd libreoffice-sdk-examples
./build.sh
```

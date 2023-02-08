## GSTH266Enc: A Gstreamer Plugin for VVC Encoder
An open-source GStreamer Plugin for VVC encoder (h266enc)

This plugin is still under development and will continue to improve by:

1. extending the API layer to support other known or future VVC encoder implementations
2. exposing additional encoder properties 
3. extending support to other platforms (e.g. linux)
4. ... 

## Plugin Architecture 
Following is a pictorial representation of the Gstreamer plugin architecture

![architecture diagram](/images/architecture.png)

## Prerequisites

Prerequisites for building the Gstreamer plugin on Windows are:

- Visual Studio 19 (for MSVC compiler (version 19.29.30038.1))
- meson (version 0.59.0)
- Gstreamer (version 1.20.1)
- VVC encoder library 

VVC encoder libraries currently supported by the plugin are:
  - VVenC v1.4.0 - https://github.com/fraunhoferhhi/vvenc
  - UVG266 v0.4.1 - https://github.com/ultravideo/uvg266


## Installation

### MSVC compiler
The MSVC compiler is available as part of the visual studio for windows installation. A different C/C++ compiler for windows can be used but is not tested.  

### Meson

To install Meson, follow the instructions on https://mesonbuild.com/Getting-meson.html

Python 3.7 or up is required for meson. 

### GStreamer
1. Download the Runtime and Development installer. The installers are available under the following [link](https://gstreamer.freedesktop.org/data/pkg/windows/). In this project we are using x64.

    - We are developing this project against Gstreamer 1.20.1. To download the installers for Gstreamer 1.20.1 show the [link](https://gstreamer.freedesktop.org/data/pkg/windows/1.20.1/msvc/)

    - More information on installation of Gstreamer on Windows is available on the [official website](https://gstreamer.freedesktop.org/documentation/installing/on-windows.html#download-and-install-gstreamer-binaries)


2. Run the installers. You will be prompted with details on where to install `gstreamer`, usually it is in `C:\`

    - running the devel package will prompt you with three options ie. `Typical`, `Custom`, `Complete` 

    - choose the complete installation to install pkgconfig files 


3. Setting the environment varibles: (assuming gstreamer is installed in C:\)

    - Add `C:\gstreamer\1.0\msvc_x86_64\bin` to PATH

    - Set `PKG_CONFIG_PATH` to `C:\gstreamer\1.0\msvc_x86_64\lib\pkgconfig` 

    - Set `GSTREAMER_ROOT_X86_64` to `C:\gstreamer\1.0\msvc_x86_64`


4. Reboot your system

5. Open terminal and check `gst-launch-1.0`

### VVC encoder library

#### VVenC
- Checkout the latest code from [here](https://github.com/fraunhoferhhi/vvenc). 
- Follow the [build](https://github.com/fraunhoferhhi/vvenc/wiki/Build) process to generate the shared library (vvenc.dll)
- install the vvenc.dll to the GStreamer install directory under `C:\gstreamer\1.0\msvc_x86_64\bin\`

#### UVG266
- Checkout the latest code from [here](https://github.com/ultravideo/uvg266)
- Follow the [build](https://github.com/ultravideo/uvg266#compiling-uvg266) process to generate the shared library (uvg266.dll)
- install the generated uvg266.dll to the Gstreamer install directory under `C:\gstreamer\1.0\msvc_x86_64\bin\`


## Compiling the GSTH266enc plugin

Plugin is compiled using meson build system. The plugin is only tested on a 64-bit windows machine. 

Run the following commands in the root directory to generate the plugin and API libraries

```
    mkdir builddir
    cd builddir
    meson ../. 
        -DENCODER_TYPE=<VVENC or UVG>
        -DENCODER_LIB_PATH=<path to encoder lib directory>
        -DENCODER_INCLUDE_PATH=<path to encoder include directory>
        
    meson compile -C .
```

For example, to configure the meson to compile the plugin with VVenC library: 
```
meson ../. -DENCODER_TYPE=VVENC -DENCODER_LIB_PATH=<path to VVenC lib folder> -DENCODER_INCLUDE_PATH=<path to VVenC include folder>
```

Similarly, to configure the meson to compile the plugin with UVG266 library: 
```
meson ../. -DENCODER_TYPE=UVG -DENCODER_LIB_PATH=<path to UVG266 lib folder> -DENCODER_INCLUDE_PATH=<path to UVG266 include folder>
```

After successful compilation, following shared libraries are generated that need to be copied to the Gstreamer installation directory:
- gsth266enc.dll (under builddir/plugin/ directory)
    - This is copied to the GStreamer-1.0 lib directory at `C:\gstreamer\1.0\msvc_x86_64\lib\gstreamer-1.0\`
- <encoder_type>_lib.dll depending on the encoder type (under builddir/encoders/<encoder_type>_utils/) where encoder_type={vvenc or uvg}
    - This is copied to the GStreamer-1.0 bin directory at `C:\gstreamer\1.0\msvc_x86_64\bin\`

In the next section, we demonstrate the usage of the plugin

## Usage of the plugin

One can inspect the h266enc element using the `gst-inspect-1.0` utility provided by Gstreamer as follows

```
    gst-inspect-1.0 h266enc
```
Following are the pad templates supported by the plugin:

```
Pad Templates:
  SINK template: 'sink'
    Availability: Always
    Capabilities:
      video/x-raw
              framerate: [ 0/1, 2147483647/1 ]
                  width: [ 1, 2147483647 ]
                 height: [ 1, 2147483647 ]
                 format: { (string)I420, (string)I420_10LE }

  SRC template: 'src'
    Availability: Always
    Capabilities:
      video/x-h266
              framerate: [ 0/1, 2147483647/1 ]
                  width: [ 16, 2147483647 ]
                 height: [ 16, 2147483647 ]
          stream-format: byte-stream
              alignment: au
                profile: { (string)main-10 }

```

Following are the supported element properties:
```
    Element Properties:
        bitrate             : Bitrate in kbit/sec
                                flags: readable, writable, changeable in NULL, READY, PAUSED or PLAYING state
                                Unsigned Integer. Range: 1 - 102400000 Default: 10000000
        min-force-key-unit-interval: Minimum interval between force-keyunit requests in nanoseconds
                                flags: readable, writable
                                Unsigned Integer64. Range: 0 - 18446744073709551615 Default: 0
        name                : The name of the object
                                flags: readable, writable, 0x2000
                                String. Default: "h266enc0"
        parent              : The parent of the object
                                flags: readable, writable, 0x2000
                                Object of type "GstObject"
        qos                 : Handle Quality-of-Service events from downstream
                                flags: readable, writable
                                Boolean. Default: false
        qp                  : QP for P slices in (implied) CQP mode (-1 = disabled)
                                flags: readable, writable
                                Integer. Range: -1 - 51 Default: 32

```


To test the element, a GStreamer tool named ‘gst-launch’ can be used to test a simple pipeline as shown in figure below. 

![example pipeline](/images/example-pipeline.png)

Note that in the above case the GST plugin is configured to use the VVenC encoder. The above pipeline can be tested using the following command:

```
    gst-launch-1.0 videotestsrc ! video/x-raw,format=I420_10LE, width=256,height=256 ! h266enc bitrate=1000000 ! filesink   location=videotestsrc.266
```


## License
GstH266Enc is licensed under the BSD 3-Clause Clear License







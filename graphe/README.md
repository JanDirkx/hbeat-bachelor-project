OBS plugin
==========

This plugin implements an OBS filter which continuously sends the frame number of the recorded video to LSL in order to synchronize the video with other signals.

## Compiling
### Prerequisites
- CMake
- [liblsl](https://github.com/sccn/liblsl/releases/) (we have tested with version 1.13.1)
- a working development environment for OBS Studio installed on your computer.

### Windows
1. Build obs-studio following the [instructions](https://obsproject.com/wiki/install-instructions#windows-build-directions)
2. clone this repository
3. make a directory called build in the root of this repository
4. launch cmake-gui
  - set the source to the root of this repository
  - set the build to the build directory
  - you'll have to set these CMake variables (bold ones need to be set explicitly, the others are usually found automatically):
    - **LIBOBS_DIR** (path) : location of the libobs subfolder in the build of OBS studio
    - **OBS_FRONTEND_LIB** (filepath) : location of the obs-frontend-api.lib file
    - LSL\_DIR (path) : location of liblsl share/LSL directory
5. configure, and if everything is okay then generate the build files
6. use the build tool you selected in cmake-gui to compile the code
7. copy the `lsl_plugin.dll` to the appropriate folder in `<obs-studio-install-dir>/obs-plugins/`
8. copy `liblsl32/64.dll` to the same directory
9. also copy the contents of the `obsplugin/data/` directory to the `bin` directory of obs-studio (where the obs-studio.exe is located)

### Linux
On Debian/Ubuntu :  
```
git clone https://gitlab.unige.ch/sims/lsl-modules/obs-plugin.git
cd obs-plugin
mkdir build && cd build
cmake (-DLIBOBS_INCLUDE_DIR="<path to the libobs sub-folder in obs-studio's source code>")(voir si toujours utile) -DCMAKE_INSTALL_PREFIX=/usr ..
make -j4
sudo make install(ne marche pas et en aie pas eux besoin)
```
1. Copy the `lsl_plugin.so` to /usr/lib/obs-plugins/
2. also copy the contents of the `obsplugin/data/` directory to the `bin` directory of obs-studio (where the obs-studio executable is located)
(pour l'instant il faut lancer obs depuis le bin)
## Using the plugin
This plugin implements a source filter in obs. To use it select the **scene** containing the sources you wish to record and add the LSL filter to it.
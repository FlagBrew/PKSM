### Unix
First you will need [devkitArm] which can be obtained with

```bash
wget http://sourceforge.net/projects/devkitpro/files/Automated%20Installer/devkitARMupdate.pl
chmod +x devkitARMupdate.pl
sudo ./devkitARMupdate.pl /opt/devkitPro
echo "export DEVKITPRO=/opt/devkitPro" >> ~/.bashrc
echo "export DEVKITARM=\$DEVKITPRO/devkitARM" >> ~/.bashrc
echo "export PATH=\$PATH:\$DEVKITARM/bin" >> ~/.bashrc
source ~/.bashrc
```

Please note: The env variables need to be available from sudo

```bash
Defaults env_keep += "DEVKITPRO DEVKITARM"
```

The following can be installed with [3ds_portlibs]:
* libpng 1.6.19
* libJPEGTurbo 1.5.1
* freetype 2.7.1

The following will need to be manually installed:
* [sf2dlib]
* [sftdlib]
* [sfillib]

When cloning the repo make sure to use `git clone --recursive` in order to also get buildtools and the other dependencies.

In case you're compiling for *hax-based homebrew launchers ensure you also create an xml file with 
`<targets selectable="true"></targets>` and put it along side the `.3dsx` file with matching names.

### Windows
Install the following using [devkitProUpdater]:
* [devkitArm] r47
* [libctru] 1.3.0
* [citro3D] 1.3.0

**Note**: It is suggested to install dev tools at the root of your disk drive i.e. `C:\Development\devkitPro`

Once installed, make sure you have access to a C compiler from `cmd`. If you're not sure install [mingw-w64] to 
`C:\Development\mingw-w64` and then add the path `C:\Development\mingw-w64\mingw32\bin` to your env variables.

Open command prompt as an **administrator** and `cd` into the devkitPro installation folder. Some commands need admin 
rights to properly install.

Next `git clone` the latest [3ds_portlibs]:
* zlib
* libpng 1.6.19
* libJPEGTurbo 1.5.1
* freetype 2.7.1

```bash
git clone https://github.com/devkitPro/3ds_portlibs.git
cd 3ds_portlibs

make zlib
make install-zlib

make libpng
make install libpng

make libjpeg-turbo
make install libjpeg-turbo

make freetype
make install freetype
```

Manually install each of the following into the devkitPro folder:
* [sf2dlib]
* [sftdlib]
* [sfillib]

```bash
git clone https://github.com/xerpi/sf2dlib.git
cd sf2dlib/libsf2d
make
make install

git clone https://github.com/xerpi/sftdlib.git
cd sftdlib/libsftd
make
make install

git clone https://github.com/xerpi/sfillib.git
cd sfillib
make
make install
```

Finally, clone this repo recursively and build with

```bash
git clone --recursive https://github.com/BernardoGiordano/PKSM.git
cd PKSM
make
make install
```

In case you're compiling for *hax-based homebrew launchers ensure you also create an xml file with 
`<targets selectable="true"></targets>` and put it along side the `.3dsx` file with matching names.

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. 
There is no need to format nicely because it shouldn't be seen. 
Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)

[3ds_portlibs]: <https://github.com/devkitPro/3ds_portlibs>
[devkitProUpdater]: <https://sourceforge.net/projects/devkitpro/>
[devkitArm]: <https://sourceforge.net/projects/devkitpro/files/devkitARM/>
[citro3D]: <https://sourceforge.net/projects/devkitpro/files/citro3d/>
[libctru]: <https://sourceforge.net/projects/devkitpro/files/libctru/>
[mingw-w64]: <https://sourceforge.net/projects/mingw-w64/>
[sf2dlib]: <https://github.com/xerpi/sf2dlib>
[sftdlib]: <https://github.com/xerpi/sftdlib>
[sfillib]: <https://github.com/xerpi/sfillib>

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

When cloning the repo make sure to use `git clone --recursive` in order to also get pp2d.

In case you're compiling for *hax-based homebrew launchers ensure you also create an xml file with 
`<targets selectable="true"></targets>` and put it along side the `.3dsx` file with matching names.

### Windows
Install the following using [devkitProUpdater]:
* [devkitArm] r47
* [libctru] 1.3.0
* [citro3D] 1.3.0

**Note**: It is suggested to install dev tools at the root of your disk drive i.e. `C:\devkitPro`

Once installed, make sure you have access to a C compiler from `cmd`.

Finally, clone this repo recursively and build with one of the following build options:

```bash
git clone --recursive https://github.com/BernardoGiordano/PKSM.git
cd PKSM

make all
```

In case you're compiling for *hax-based homebrew launchers ensure you also create an xml file with 
`<targets selectable="true"></targets>` and put it along side the `.3dsx` file with matching names.

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. 
There is no need to format nicely because it shouldn't be seen. 
Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)

[devkitProUpdater]: <https://sourceforge.net/projects/devkitpro/>
[devkitArm]: <https://sourceforge.net/projects/devkitpro/files/devkitARM/>
[citro3D]: <https://sourceforge.net/projects/devkitpro/files/citro3d/>
[libctru]: <https://sourceforge.net/projects/devkitpro/files/libctru/>

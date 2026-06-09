#!/usr/bin/python3
import platform
import urllib.request
import os
import stat

def main():
    if platform.system() == "Windows":
        binary = "gallerypack-windows.exe"
        execute = binary
        # Windows 最后可用版本：1.2.0
        download_url = f"https://github.com/FlagBrew/EventsGalleryPacker/releases/download/1.2.0/gallerypack-windows.exe"
    elif platform.system() == "Darwin":
        binary = "gallerypack-mac"
        execute = "./" + binary
        # macOS 最后可用版本：1.1.1
        download_url = f"https://github.com/FlagBrew/EventsGalleryPacker/releases/download/1.1.1/gallerypack-mac"
    elif platform.system() == "Linux":
        binary = "gallerypack-linux"
        execute = "./" + binary
        # Linux 仍使用最新版
        download_url = f"https://github.com/FlagBrew/EventsGalleryPacker/releases/latest/download/gallerypack-linux"
    else:
        print("This system is not supported")
        return 1

    with urllib.request.urlopen( download_url ) as response, open(binary, 'wb') as out_file:
        data = response.read()
        out_file.write(data)
    
    # chmod 755
    os.chmod(binary, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)

    os.system(execute + " ./EventsGallery ../../core/personals")
    
if __name__ == "__main__":
    main()

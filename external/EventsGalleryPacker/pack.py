#!/usr/bin/python3
import platform
import urllib.request
import os
import stat

def main():
    if platform.system() == "Windows":
        binary = "gallerypack-windows.exe"
        execute = binary
    elif platform.system() == "Darwin":
        binary = "gallerypack-mac"
        execute = "./" + binary
    elif platform.system() == "Linux":
        binary = "gallerypack-linux"
        execute = "./" + binary
    else:
        print("This system is not supported")
        return 1

    with urllib.request.urlopen("https://github.com/FlagBrew/EventsGalleryPacker/releases/latest/download/" + binary) as response, open(binary, 'wb') as out_file:
        data = response.read()
        out_file.write(data)
    
    # chmod 755
    os.chmod(binary, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)

    os.system(execute + " ./EventsGallery")
    
if __name__ == "__main__":
    main()

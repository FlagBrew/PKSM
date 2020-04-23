#!/usr/bin/python3
import platform
import urllib.request
import os
import git
from shutil import rmtree
import stat

def main():
    if platform.system() == "Windows":
        binary = "gallerypack-windows.exe"
    elif platform.system() == "Darwin":
        binary = "gallerypack-mac"
    elif platform.system() == "Linux":
        binary = "gallerypack-linux"
    else:
        print("This system is not supported")
        return 1

    with urllib.request.urlopen("https://github.com/FlagBrew/EventsGalleryPacker/releases/latest/download/" + binary) as response, open("gallerypack", 'wb') as out_file:
        data = response.read()
        out_file.write(data)
    
    # chmod 755
    os.chmod("gallerypack", stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)
    
    if os.path.exists("./EventsGallery"):
        print("Pulling from EventsGallery...")
        try:
            repo = git.Repo("./EventsGallery")
            repo.remotes.origin.pull()
        except git.InvalidGitRepositoryError:
            print("Repository corrupted! Cloning EventsGallery...")
            rmtree('./EventsGallery')
            try:
                git.Git(".").clone("https://github.com/projectpokemon/EventsGallery.git")
            except git.GitCommandError:
                print("Could not clone EventsGallery. Aborting...")
                exit(1)
        except git.GitCommandError:
            print("Error while pulling! Continuing with existing repo")
    else:
        print("Cloning EventsGallery...")
        try:
            git.Git(".").clone("https://github.com/projectpokemon/EventsGallery.git")
        except git.GitCommandError:
            print("Could not clone EventsGallery. Aborting...")
            exit(1)

    os.system("./gallerypack ./EventsGallery")
    
if __name__ == "__main__":
    main()

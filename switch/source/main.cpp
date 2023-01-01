
#include <stdio.h>
#include <string.h>

#include <switch.h>

// Borrowed from the switch examples repo, just trying to get a grip on switch development.
int main(int argc, char** argv)
{
    // Initialize console. Using NULL as the second argument tells the console library to use the
    // internal console structure as current one.
    consoleInit(NULL);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first
    // connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    // Move the cursor to row 16 and column 20 and then prints "Hello World!"
    // To move the cursor you have to print "\x1b[r;cH", where r and c are respectively
    // the row and column where you want your cursor to move
    printf("\x1b[16;20HHello World!");

    while (appletMainLoop())
    {
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        // padGetButtonsDown returns the set of buttons that have been newly pressed in this frame
        // compared to the previous one
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
        {
            break; // break in order to return to hbmenu
        }

        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}
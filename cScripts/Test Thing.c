#include <picoc_unix.h>

int main()
{
	int randNum = 47;
    gui_fpsCheck(randNum % 807 + 1, 0, 7, randNum % 400, randNum % 240, 1);
    return 0;
}
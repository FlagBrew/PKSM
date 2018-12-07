#include "interpreter.h"
#include "scripthelpers.h"

// static const int FOUR = 0;
// static const int FIVE = 1;
// static const int SIX = 2;
// static const int SEVEN = 3;

void UnixSetupFunc()
{
}

void Ctest (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    printf("test(%d)\n", Param[0]->Val->Integer);
    Param[0]->Val->Integer = 1234;
}

void Clineno (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    ReturnValue->Val->Integer = Parser->Line;
}

/* list of all library functions and their prototypes */
struct LibraryFunction UnixFunctions[] =
{
    { Ctest,            "void test(int);" },
    { Clineno,          "int lineno();" },
    { gui_fpsCheck,     "int gui_fpsCheck();" },
    { gui_choice,       "int gui_choice(char* lineOne, char* lineTwo);" },
    { gui_warn,         "void gui_warn(char* lineOne, char* lineTwo);"},
    { gui_menu6x5,      "int gui_menu6x5(char* question, int options, char** labels, struct pkx* pokemon, enum Generation generation);" },
    { gui_menu20x2,     "int gui_menu20x2(char* question, int options, char** labels);" },
    { gui_keyboard,     "void gui_keyboard(char* out, char* hint, int maxChars);" },
    { gui_numpad,       "void gui_numpad(int* out, int maxDigits);" },
    { sav_sbo,          "int sav_sbo();" },
    { sav_gbo,          "int sav_gbo();" },
    { sav_boxDecrypt,   "void sav_boxDecrypt();" },
    { sav_boxEncrypt,   "void sav_boxEncrypt();" },
    { NULL,             NULL }
};

void PlatformLibraryInit(Picoc *pc)
{
    IncludeRegister(pc, "picoc_unix.h", &UnixSetupFunc, &UnixFunctions[0], "struct pkx { int species; int form; }; enum Generation { GEN_FOUR, GEN_FIVE, GEN_SIX, GEN_SEVEN, GEN_LGPE };");
}

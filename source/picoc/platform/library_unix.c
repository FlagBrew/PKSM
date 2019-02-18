#include "interpreter.h"
#include "pksm_api.h"

void UnixSetupFunc() { }

/* list of all library functions and their prototypes */
struct LibraryFunction UnixFunctions[] =
{
    // gui
    { gui_choice,       "int gui_choice(char* lineOne, char* lineTwo);" },
    { gui_warn,         "void gui_warn(char* lineOne, char* lineTwo);"},
    { gui_menu6x5,      "int gui_menu_6x5(char* question, int options, char** labels, struct pkx* pokemon, enum Generation generation);" },
    { gui_menu20x2,     "int gui_menu_20x2(char* question, int options, char** labels);" },
    { gui_keyboard,     "void gui_keyboard(char* out, char* hint, int maxChars);" },
    { gui_numpad,       "void gui_numpad(int* out, char* hint, int maxDigits);" },
    { gui_boxes,        "int gui_boxes(int* fromStorage, int* box, int* slot);" },
    // sav and storage
    { sav_sbo,          "int sav_sbo();" },
    { sav_gbo,          "int sav_gbo();" },
    { sav_boxDecrypt,   "void sav_box_decrypt();" },
    { sav_boxEncrypt,   "void sav_box_encrypt();" },
    { sav_get_pkx,      "void sav_get_pkx(char* data, int box, int slot);" },
    { sav_inject_pkx,   "void sav_inject_pkx(char* data, enum Generation type, int box, int slot);" },
    { sav_inject_ekx,   "void sav_inject_ekx(char* data, enum Generation type, int box, int slot);" },
    { party_get_pkx,    "void party_get_pkx(char* data, int slot);" },
    { bank_inject_pkx,  "void bank_inject_pkx(char* data, enum Generation type, int box, int slot);" },
    // io
    { current_directory,"char* current_directory();" },
    { read_directory,   "struct directory* read_directory(char* dir);" },
    // configurations
    { cfg_default_ot,   "char* cfg_default_ot();" },
    { cfg_default_tid,  "unsigned int cfg_default_tid();" },
    { cfg_default_sid,  "unsigned int cfg_default_sid();" },
    { cfg_default_day,  "int cfg_default_day();" },
    { cfg_default_month,"int cfg_default_month();" },
    { cfg_default_year, "int cfg_default_year();" },
    // networking
    { net_ip,           "char* net_ip();" },
    { net_tcp_receiver, "int net_tcp_recv(char* buffer, int size, int* received);" },
    { net_tcp_sender,   "int net_tcp_send(char* ip, int port, char* buffer, int size);" },
    { net_udp_receiver, "int net_udp_recv(char* buffer, int size, int* received);" },
    // i18n
    { i18n_species,     "const char* i18n_species(int species);" },
    // end
    { NULL,             NULL }
};

void PlatformLibraryInit(Picoc *pc)
{
    IncludeRegister(pc, "pksm.h", &UnixSetupFunc, &UnixFunctions[0], "struct pkx { int species; int form; }; enum Generation { GEN_FOUR, GEN_FIVE, GEN_SIX, GEN_SEVEN, GEN_LGPE }; struct directory { int count; char** files; };");
}

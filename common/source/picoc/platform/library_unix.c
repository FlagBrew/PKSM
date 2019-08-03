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
    { gui_boxes,        "int gui_boxes(int* fromStorage, int* box, int* slot, int doCrypt);" },
    // sav and storage
    { sav_sbo,          "int sav_sbo();" },
    { sav_gbo,          "int sav_gbo();" },
    { sav_boxDecrypt,   "void sav_box_decrypt();" },
    { sav_boxEncrypt,   "void sav_box_encrypt();" },
    { sav_get_pkx,      "void sav_get_pkx(char* data, int box, int slot);" },
    { sav_inject_pkx,   "void sav_inject_pkx(char* data, enum Generation type, int box, int slot, int doTradeEdits);" },
    { sav_get_sid,      "unsigned short sav_get_sid();" },
    { sav_get_tid,      "unsigned short sav_get_tid();" },
    { sav_get_ot_name,  "char* sav_get_ot_name();" },
    { party_get_pkx,    "void party_get_pkx(char* data, int slot);" },
    { party_inject_pkx, "void party_inject_pkx(char* data, enum Generation type, int slot);" },
    { bank_inject_pkx,  "void bank_inject_pkx(char* data, enum Generation type, int box, int slot);" },
    // pkm
    { pkx_encrypt,      "void pkx_decrypt(char* data, enum Generation type);" },
    { pkx_decrypt,      "void pkx_encrypt(char* data, enum Generation type);" },
    { pkx_box_size,     "int pkx_box_size(enum Generation gen);" },
    { pkx_party_size,   "int pkx_party_size(enum Generation gen);" },
    { pkx_generate,     "void pkx_generate(char* data, int species);" },
    { pkx_set_ot_name,  "void pkx_set_ot_name(char* data, enum Generation gen, char* name);" },
    { pkx_set_tid,      "void pkx_set_tid(char* data, enum Generation gen, unsigned short id);" },
    { pkx_set_sid,      "void pkx_set_sid(char* data, enum Generation gen, unsigned short id);" },
    { pkx_is_valid,     "int pkx_is_valid(char* data, enum Generation gen);" },
    { pkx_set_shiny,    "void pkx_set_shiny(char* data, enum Generation gen, int shiny);" },
    { pkx_set_language, "void pkx_set_language(char* data, enum Generation gen, int lang);" },
    // io
    { current_directory,"char* current_directory();" },
    { read_directory,   "struct directory* read_directory(char* dir);" },
    // configurations
    { cfg_default_ot,   "char* cfg_default_ot();" },
    { cfg_default_tid,  "unsigned short cfg_default_tid();" },
    { cfg_default_sid,  "unsigned short cfg_default_sid();" },
    { cfg_default_day,  "int cfg_default_day();" },
    { cfg_default_month,"int cfg_default_month();" },
    { cfg_default_year, "int cfg_default_year();" },
    // networking
    { net_ip,           "char* net_ip();" },
    { net_tcp_receiver, "int net_tcp_recv(char* buffer, int size, int* received);" },
    { net_tcp_sender,   "int net_tcp_send(char* ip, int port, char* buffer, int size);" },
    { net_udp_receiver, "int net_udp_recv(char* buffer, int size, int* received);" },
    // i18n
    { i18n_species,     "char* i18n_species(int species);" },
    // text conversion
    { pksm_utf16_to_utf8, "char* utf16_to_utf8(char* data);" },
    { pksm_utf8_to_utf16, "char* utf8_to_utf16(char* data);" },
    // end
    { NULL,             NULL }
};

void PlatformLibraryInit(Picoc *pc)
{
    IncludeRegister(pc, "pksm.h", &UnixSetupFunc, &UnixFunctions[0], "struct pkx { int species; int form; }; enum Generation { GEN_FOUR, GEN_FIVE, GEN_SIX, GEN_SEVEN, GEN_LGPE }; struct directory { int count; char** files; };");
}

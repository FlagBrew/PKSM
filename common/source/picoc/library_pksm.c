#include "interpreter.h"
#include "pksm_api.h"

void UnixSetupFunc() {}

// clang-format off
/* list of all library functions and their prototypes */
struct LibraryFunction UnixFunctions[] =
{
    // gui
    { gui_choice,           "int gui_choice(char* message);" },
    { gui_warn,             "void gui_warn(char* warning);" },
    { gui_splash,           "void gui_splash(char* notification);" },
    { gui_menu6x5,          "int gui_menu_6x5(char* question, int options, char** labels, struct pkx* pokemon, enum Generation generation);" },
    { gui_menu20x2,         "int gui_menu_20x2(char* question, int options, char** labels);" },
    { gui_keyboard,         "void gui_keyboard(char* out, char* hint, int maxChars);" },
    { gui_numpad,           "void gui_numpad(unsigned int* out, char* hint, int maxDigits);" },
    { gui_boxes,            "int gui_boxes(int* fromStorage, int* box, int* slot, int doCrypt);" },
    // sav and storage
    { sav_gbo,              "int sav_gbo(void);" },
    { sav_sbo,              "int sav_sbo(void);" },
    { sav_boxDecrypt,       "void sav_box_decrypt(void);" },
    { sav_boxEncrypt,       "void sav_box_encrypt(void);" },
    { sav_get_pkx,          "void sav_get_pkx(char* data, int box, int slot);" },
    { sav_inject_pkx,       "void sav_inject_pkx(char* data, enum Generation type, int box, int slot, int doTradeEdits);" },
    { sav_inject_wcx,       "void sav_inject_wcx(char* data, enum Generation type, int slot, int alternateFormat);" },
    { sav_wcx_free_slot,    "int sav_wcx_free_slot(void);" },
    { sav_get_value,        "int sav_get_value(enum SAV_Field field, ...);" },
    { sav_get_max,          "int sav_get_max(enum SAV_MaxField field, ...);" },
    { sav_check_value,      "int sav_check_value(enum SAV_CheckValue field, int value);" },
    { sav_register_pkx_dex, "void sav_register_pkx_dex(char* data, enum Generation gen);"},
    { party_get_pkx,        "void party_get_pkx(char* data, int slot);" },
    { party_inject_pkx,     "void party_inject_pkx(char* data, enum Generation type, int slot);" },
    { bank_inject_pkx,      "void bank_inject_pkx(char* data, enum Generation type, int box, int slot);" },
    { bank_get_pkx,         "char* bank_get_pkx(enum Generation* type, int box, int slot);" },
    { bank_get_size,        "int bank_get_size(void);" },
    { bank_select,          "void bank_select(void);" },
    // general data handling
    { sav_get_data,         "void sav_get_data(char* dataOut, unsigned int size, int off1, int off2);" },
    { sav_set_data,         "void sav_set_data(char* data, unsigned int size, int off1, int off2);" },
    { sav_get_bit,          "int sav_get_bit(int off1, int off2, int bit);" },
    { sav_set_bit,          "void sav_set_bit(int bitVal, int off1, int off2, int bit);" },
    { sav_get_byte,         "char sav_get_byte(int off1, int off2);" },
    { sav_set_byte,         "void sav_set_byte(char data, int off1, int off2);" },
    { sav_get_short,        "short sav_get_short(int off1, int off2);" },
    { sav_set_short,        "void sav_set_short(short data, int off1, int off2);" },
    { sav_get_int,          "int sav_get_int(int off1, int off2);" },
    { sav_set_int,          "void sav_set_int(int data, int off1, int off2);" },
    { sav_get_string,       "char* sav_get_string(int off1, int off2, unsigned int codepoints);" },
    { sav_set_string,       "void sav_set_string(char* string, int off1, int off2, unsigned int codepoints);" },
    { sav_get_palpark,       "int sav_get_palpark(char** out, int* outSize);" },
    { sav_set_palpark,       "int sav_set_palpark(char** data, enum Generation* gen, int inSize);" },
    // pkm
    { pkx_encrypt,          "void pkx_decrypt(char* data, enum Generation type, int isParty);" },
    { pkx_decrypt,          "void pkx_encrypt(char* data, enum Generation type, int isParty);" },
    { pkx_box_size,         "int pkx_box_size(enum Generation gen);" },
    { pkx_party_size,       "int pkx_party_size(enum Generation gen);" },
    { pkx_generate,         "void pkx_generate(char* data, int species);" },
    { pkx_is_valid,         "int pkx_is_valid(char* data, enum Generation gen);" },
    { pkx_set_value,        "void pkx_set_value(char* data, enum Generation gen, enum PKX_Field field, ...);" },
    { pkx_get_value,        "unsigned int pkx_get_value(char* data, enum Generation gen, enum PKX_Field field, ...);" },
    { pkx_update_party_data, "void pkx_update_party_data(char* data, enum Generation gen);"},
    { pkx_ribbon_exists,    "int pkx_ribbon_exists(enum Generation gen, enum Ribbon ribbon);"},
    { pkx_get_ribbon,       "int pkx_get_ribbon(char* data, enum Generation gen, enum Ribbon ribbon);"},
    { pkx_set_ribbon,       "void pkx_set_ribbon(char* data, enum Generation gen, enum Ribbon ribbon, int hasRibbon);"},
    // io
    { current_directory,    "char* current_directory(void);" },
    { read_directory,       "struct directory* read_directory(char* dir);" },
    { delete_directory,     "void delete_directory(struct directory* dir);" },
    { save_path,            "char* save_path(void);" },
    // configurations
    { cfg_default_ot,       "char* cfg_default_ot(enum Generation gen);" },
    { cfg_default_tid,      "unsigned short cfg_default_tid(enum Generation gen);" },
    { cfg_default_sid,      "unsigned short cfg_default_sid(enum Generation gen);" },
    { cfg_default_day,      "int cfg_default_day(void);" },
    { cfg_default_month,    "int cfg_default_month(void);" },
    { cfg_default_year,     "int cfg_default_year(void);" },
    // networking
    { net_ip,               "char* net_ip(void);" },
    { net_tcp_receiver,     "int net_tcp_recv(char* buffer, int size, int* received);" },
    { net_tcp_sender,       "int net_tcp_send(char* ip, int port, char* buffer, int size);" },
    { net_udp_receiver,     "int net_udp_recv(char* buffer, int size, int* received);" },
    { fetch_web_content,    "int fetch_web_content(char** out, int* outSize, char* url);" },
    // i18n
    { i18n_species,         "char* i18n_species(int species);" },
    { i18n_form,            "char* i18n_form(int gameVersion, int species, int form);" },
    // text conversion
    { pksm_ucs2_to_utf8,   "char* ucs2_to_utf8(char* data);" },
    { pksm_utf8_to_ucs2,   "char* utf8_to_ucs2(char* data);" },
    // misc
    { pksm_base64_decode,   "void base64_decode(unsigned char** out, int* outSize, char* data, int size);" },
    { pksm_base64_encode,   "void base64_encode(char** out, int* outSize, unsigned char* data, int size);" },
    { pksm_bz2_compress,    "int bz2_compress(unsigned char** out, int* outSize, unsigned char* data, int size);" },
    { pksm_bz2_decompress,  "int bz2_decompress(unsigned char** out, int* outSize, unsigned char* data, int size);" },
    { pksm_get_max_pp,      "int max_pp(enum Generation gen, int move, int ppUps);" },
    // json
    { json_new,             "struct JSON* json_new(void);" },
    { json_parse,           "void json_parse(struct JSON* out, char* data);" },
    { json_delete,          "void json_delete(struct JSON* freed);" },
    { json_is_valid,        "int json_is_valid(struct JSON* check);" },
    { json_is_int,          "int json_is_int(struct JSON* check);" },
    { json_is_bool,         "int json_is_bool(struct JSON* check);" },
    { json_is_string,       "int json_is_string(struct JSON* check);" },
    { json_is_array,        "int json_is_array(struct JSON* check);" },
    { json_is_object,       "int json_is_object(struct JSON* check);" },
    { json_get_int,         "int json_get_int(struct JSON* get);" },
    { json_get_bool,        "int json_get_bool(struct JSON* get);" },
    { json_get_string,      "char* json_get_string(struct JSON* get);" },
    { json_array_size,      "int json_array_size(struct JSON* get);" },
    { json_array_element,   "struct JSON* json_array_element(struct JSON* array, int index);" },
    { json_object_contains, "int json_object_contains(struct JSON* get, char* elemName);" },
    { json_object_element,  "struct JSON* json_object_element(struct JSON* object, char* elemName);" },
    // end
    { NULL,                 NULL }
};

void PlatformLibraryInit(Picoc *pc)
{
    IncludeRegister(pc, "pksm.h", &UnixSetupFunc, &UnixFunctions[0],
    "struct pkx { int species; int form; };"
    "struct JSON { void* dummy; };"
    "enum Generation { GEN_FOUR, GEN_FIVE, GEN_SIX, GEN_SEVEN, GEN_LGPE, GEN_EIGHT, GEN_THREE, GEN_ONE, GEN_TWO };"
    "struct directory { int count; char** files; };"
    "enum PKX_Field {OT_NAME, TID, SID, SHINY, LANGUAGE, MET_LOCATION, MOVE, BALL, LEVEL, GENDER,"
                    "ABILITY, IV_HP, IV_ATK, IV_DEF, IV_SPATK, IV_SPDEF, IV_SPEED, NICKNAME, ITEM,"
                    "POKERUS, EGG_DAY, EGG_MONTH, EGG_YEAR, MET_DAY, MET_MONTH, MET_YEAR, FORM,"
                    "EV_HP, EV_ATK, EV_DEF, EV_SPATK, EV_SPDEF, EV_SPEED, SPECIES, PID, NATURE, FATEFUL,"
                    "PP, PP_UPS, EGG, NICKNAMED, EGG_LOCATION, MET_LEVEL, OT_GENDER, ORIGINAL_GAME};"
    "enum SAV_Field { SAV_OT_NAME, SAV_TID, SAV_SID, SAV_GENDER, SAV_COUNTRY, SAV_SUBREGION, SAV_REGION,"
                    "SAV_LANGUAGE, SAV_MONEY, SAV_BP, SAV_HOURS, SAV_MINUTES, SAV_SECONDS, SAV_ITEM };"
    "enum SAV_MaxField { MAX_SLOTS, MAX_BOXES, MAX_WONDER_CARDS, MAX_FORM, MAX_IN_POUCH };"
    "enum SAV_CheckValue { SAV_VALUE_SPECIES, SAV_VALUE_MOVE, SAV_VALUE_ITEM, SAV_VALUE_ABILITY, SAV_VALUE_BALL };"
    "enum Pouch { NormalItem, KeyItem, TM, Mail, Medicine, Berry, Ball, Battle, Candy, ZCrystals };"
    "enum Ribbon {RIB_ChampionKalos, RIB_ChampionG3Hoenn, RIB_ChampionSinnoh, RIB_BestFriends, RIB_Training, RIB_BattlerSkillful,"
                 "RIB_BattlerExpert, RIB_Effort, RIB_Alert, RIB_Shock, RIB_Downcast, RIB_Careless, RIB_Relax, RIB_Snooze, RIB_Smile, RIB_Gorgeous,"
                 "RIB_Royal, RIB_GorgeousRoyal, RIB_Artist, RIB_Footprint, RIB_Record, RIB_Legend, RIB_Country, RIB_National, RIB_Earth, RIB_World,"
                 "RIB_Classic, RIB_Premier, RIB_Event, RIB_Birthday, RIB_Special, RIB_Souvenir, RIB_Wishing, RIB_ChampionBattle, RIB_ChampionRegional,"
                 "RIB_ChampionNational, RIB_ChampionWorld, RIB_MemoryContest, RIB_MemoryBattle, RIB_ChampionG6Hoenn, RIB_ContestStar,"
                 "RIB_MasterCoolness, RIB_MasterBeauty, RIB_MasterCuteness, RIB_MasterCleverness, RIB_MasterToughness, RIB_G3Cool,"
                 "RIB_G3CoolSuper, RIB_G3CoolHyper, RIB_G3CoolMaster, RIB_G3Beauty, RIB_G3BeautySuper, RIB_G3BeautyHyper, RIB_G3BeautyMaster,"
                 "RIB_G3Cute, RIB_G3CuteSuper, RIB_G3CuteHyper, RIB_G3CuteMaster, RIB_G3Smart, RIB_G3SmartSuper, RIB_G3SmartHyper, RIB_G3SmartMaster,"
                 "RIB_G3Tough, RIB_G3ToughSuper, RIB_G3ToughHyper, RIB_G3ToughMaster, RIB_G4Cool, RIB_G4CoolSuper, RIB_G4CoolHyper, RIB_G4CoolMaster,"
                 "RIB_G4Beauty, RIB_G4BeautySuper, RIB_G4BeautyHyper, RIB_G4BeautyMaster, RIB_G4Cute, RIB_G4CuteSuper, RIB_G4CuteHyper,"
                 "RIB_G4CuteMaster, RIB_G4Smart, RIB_G4SmartSuper, RIB_G4SmartHyper, RIB_G4SmartMaster, RIB_G4Tough, RIB_G4ToughSuper,"
                 "RIB_G4ToughHyper, RIB_G4ToughMaster, RIB_Winning, RIB_Victory, RIB_Ability, RIB_AbilityGreat, RIB_AbilityDouble, RIB_AbilityMulti,"
                 "RIB_AbilityPair, RIB_AbilityWorld, RIB_ChampionAlola, RIB_BattleRoyale, RIB_BattleTreeGreat, RIB_BattleTreeMaster,"
                 "RIB_ChampionGalar, RIB_TowerMaster, RIB_MasterRank, RIB_MarkLunchtime, RIB_MarkSleepyTime, RIB_MarkDusk, RIB_MarkDawn,"
                 "RIB_MarkCloudy, RIB_MarkRainy, RIB_MarkStormy, RIB_MarkSnowy, RIB_MarkBlizzard, RIB_MarkDry, RIB_MarkSandstorm, RIB_MarkMisty,"
                 "RIB_MarkDestiny, RIB_MarkFishing, RIB_MarkCurry, RIB_MarkUncommon, RIB_MarkRare, RIB_MarkRowdy, RIB_MarkAbsentMinded,"
                 "RIB_MarkJittery, RIB_MarkExcited, RIB_MarkCharismatic, RIB_MarkCalmness, RIB_MarkIntense, RIB_MarkZonedOut, RIB_MarkJoyful,"
                 "RIB_MarkAngry, RIB_MarkSmiley, RIB_MarkTeary, RIB_MarkUpbeat, RIB_MarkPeeved, RIB_MarkIntellectual, RIB_MarkFerocious,"
                 "RIB_MarkCrafty, RIB_MarkScowling, RIB_MarkKindly, RIB_MarkFlustered, RIB_MarkPumpedUp, RIB_MarkZeroEnergy, RIB_MarkPrideful,"
                 "RIB_MarkUnsure, RIB_MarkHumble, RIB_MarkThorny, RIB_MarkVigor, RIB_MarkSlump, RIB_G4CoolGreat = RIB_G4CoolSuper,"
                 "RIB_G4CoolUltra = RIB_G4CoolHyper, RIB_G4BeautyGreat = RIB_G4BeautySuper, RIB_G4BeautyUltra = RIB_G4BeautyHyper,"
                 "RIB_G4CuteGreat = RIB_G4CuteSuper, RIB_G4CuteUltra = RIB_G4CuteHyper, RIB_G4SmartGreat = RIB_G4SmartSuper,"
                 "RIB_G4SmartUltra = RIB_G4SmartHyper, RIB_G4ToughGreat = RIB_G4ToughSuper, RIB_G4ToughUltra = RIB_G4ToughHyper };");
}

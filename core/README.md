# PKSM-Core
The core save editing code for PKSM (https://github.com/FlagBrew/PKSM)

Usage in another project:

Special info:
- Your include path must have a file named `PKSMCORE_CONFIG.h` that includes some defines, listed below

Defines:
- _PKSMCORE_LANG_FOLDER: the folder from which languages are loaded
- _PKSMCORE_GETLINE_FUNC: the name of your stdio.h getline function

Necessary include directories:
- fmt
- memecrypto
- swshcrypto
- include

Necessary source directories:
- memecrypto
- swshcrypto
- source and all its subdirectories

make
makerom -f cia -o EventAssistant/EventAssistant.cia -DAPP_ENCRYPTED=false -DAPP_COMP=true -rsf include/EventAssistant.rsf -target t -exefslogo -elf EventAssistant/EventAssistant.elf -icon include/icon.bin -banner include/banner.bin
pause
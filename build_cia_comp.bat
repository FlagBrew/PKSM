make
makerom -f cia -o ECITool.cia -DAPP_ENCRYPTED=false -DAPP_COMP=true -rsf ECITool.rsf -target t -exefslogo -elf ECITool.elf -icon icon.bin -banner banner.bin
pause
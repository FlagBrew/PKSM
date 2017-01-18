make
rd Release /s /q
mkdir Release\3ds\PKSM
mkdir Release\3ds\PKSV
mkdir Release\servepx\java
mkdir Release\servepx\python
move output\3ds-arm\PKSM.cia Release
move output\3ds-arm\3ds\PKSM\PKSM.3dsx Release\3ds\PKSM
move ..\PKSV\output\3ds-arm\PKSV.cia Release
move ..\PKSV\output\3ds-arm\3ds\PKSV\PKSV.3dsx Release\3ds\PKSV
move servepkx\java\servepkx.jar Release\servepx\java
move servepkx\java\servepkx.exe Release\servepx\java
move servepkx\java\README.md Release\servepx\java
move servepkx\python\servepkx.py Release\servepx\python
move servepkx\python\README.md Release\servepx\python
zip -r PKSM.zip Release
move PKSM.zip Release
rd output /s /q
rd ..\PKSV\output /s /q
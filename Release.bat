make
rd Release /s /q
mkdir Release\3ds\PKSM Release\3ds\PKSV Release\servepx\java Release\servepx\python
copy output\3ds-arm\PKSM.cia Release
copy output\3ds-arm\3ds\PKSM\PKSM.3dsx Release\3ds\PKSM
copy ..\PKSV\output\3ds-arm\PKSV.cia Release
copy ..\PKSV\output\3ds-arm\3ds\PKSV\PKSV.3dsx Release\3ds\PKSV
copy servepkx\java\servepkx.jar Release\servepx\java
copy servepkx\java\servepkx.exe Release\servepx\java
copy servepkx\java\README.md Release\servepx\java
copy servepkx\python\servepkx.py Release\servepx\python
copy servepkx\python\README.md Release\servepx\python
@echo ^<targets selectable="true"^>^</targets^> > Release\3ds\PKSM\PKSM.xml
@echo ^<targets selectable="true"^>^</targets^> > Release\3ds\PKSV\PKSV.xml
cd Release
zip -r PKSM.zip .
cd ..\
pause
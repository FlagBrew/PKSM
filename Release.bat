make
@rd Release /s /q
@mkdir Release\3ds\PKSM Release\3ds\PKSV Release\servepkx\java Release\servepkx\python Release\servepkx\webbrowser Release\extrastorage Release\extrastorage\badsectors Release\extrastorage\phbank2pksm  

@copy output\3ds-arm\PKSM.cia Release
@copy output\3ds-arm\3ds\PKSM\PKSM.3dsx Release\3ds\PKSM

@copy C:\Users\Bernardo\Documents\GitHub\PKSV\output\3ds-arm\PKSV.cia Release
@copy C:\Users\Bernardo\Documents\GitHub\PKSV\output\3ds-arm\3ds\PKSV\PKSV.3dsx Release\3ds\PKSV

@copy servepkx\java\servepkx.jar Release\servepkx\java
@copy servepkx\java\servepkx.exe Release\servepkx\java
@copy servepkx\java\README.md Release\servepkx\java

@copy servepkx\python\servepkx.py Release\servepkx\python
@copy servepkx\python\README.md Release\servepkx\python

@copy extrastorage\badsectors\badsectors.3dsx Release\extrastorage\badsectors

@copy extrastorage\phbank2pksm\index.html Release\extrastorage\phbank2pksm
@copy extrastorage\phbank2pksm\style.css Release\extrastorage\phbank2pksm
@copy extrastorage\phbank2pksm\README.md Release\extrastorage\phbank2pksm

@copy servepkx\webbrowser\index.html Release\servepkx\webbrowser
@copy servepkx\webbrowser\style.css Release\servepkx\webbrowser
@copy servepkx\webbrowser\README.md Release\servepkx\webbrowser

@echo ^<targets selectable="true"^>^</targets^> > Release\3ds\PKSM\PKSM.xml
@echo ^<targets selectable="true"^>^</targets^> > Release\3ds\PKSV\PKSV.xml
@cd Release
@zip -r PKSM.zip .
@cd ..\
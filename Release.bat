make pksm
make pksv
make rosalinapksm
make rosalinapksv

@rd release /s /q
@mkdir release\standard\PKSM\3ds\PKSM release\standard\PKSV\3ds\PKSV release\rosalina\3ds\PKSM release\rosalina\3ds\PKSV release\servepkx\java release\servepkx\python release\servepkx\webbrowser release\extrastorage\badsectors release\extrastorage\phbank2pksm

@copy output\PKSM\3ds-arm\PKSM.cia release\standard\PKSM
@copy output\PKSM\3ds-arm\3ds\PKSM\PKSM.3dsx release\standard\PKSM\3ds\PKSM
@echo ^<targets selectable="true"^>^</targets^> > release\standard\PKSM\3ds\PKSM\PKSM.xml

@copy output\PKSV\3ds-arm\PKSV.cia release\standard\PKSV
@copy output\PKSV\3ds-arm\3ds\PKSV\PKSV.3dsx release\standard\PKSV\3ds\PKSV
@echo ^<targets selectable="true"^>^</targets^> > release\standard\PKSV\3ds\PKSV\PKSV.xml

@copy output\rosalina\PKSV\3ds-arm\3ds\PKSV\PKSV.3dsx release\rosalina\3ds\PKSV
@copy output\rosalina\PKSM\3ds-arm\3ds\PKSM\PKSM.3dsx release\rosalina\3ds\PKSM

@copy servepkx\java\servepkx.jar release\servepkx\java
@copy servepkx\java\README.md release\servepkx\java

@copy servepkx\python\servepkx.py release\servepkx\python
@copy servepkx\python\README.md release\servepkx\python

@copy extrastorage\badsectors\badsectors.3dsx release\extrastorage\badsectors

@copy extrastorage\phbank2pksm\index.html release\extrastorage\phbank2pksm
@copy extrastorage\phbank2pksm\style.css release\extrastorage\phbank2pksm
@copy extrastorage\phbank2pksm\README.md release\extrastorage\phbank2pksm

@copy servepkx\webbrowser\index.html release\servepkx\webbrowser
@copy servepkx\webbrowser\style.css release\servepkx\webbrowser
@copy servepkx\webbrowser\README.md release\servepkx\webbrowser

@cd release
@zip -r PKSM.zip .
@cd ..\
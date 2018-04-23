#!/bin/sh

if [ -f ./BasiliskII ];
  then

	rm -f -r ./BasiliskII.app
	mkdir -p BasiliskII.app/Contents
	cp ../MacOSX/Info.plist BasiliskII.app/Contents/

	mkdir -p BasiliskII.app/Contents/MacOS
	cp BasiliskII BasiliskII.app/Contents/MacOS/

	mkdir -p BasiliskII.app/Contents/Resources
	cp ../MacOSX/Info.plist BasiliskII.app/Contents/Resources/
	cp -r ../MacOSX/English.lproj BasiliskII.app/Contents/Resources/
	cp ../MacOSX/*.icns BasiliskII.app/Contents/Resources/
	cp ../MacOSX/*.html BasiliskII.app/Contents/Resources/
	cp ../MacOSX/BasiliskII_keycodes BasiliskII.app/Contents/Resources/
	cp ../../README BasiliskII.app/Contents/Resources/README.txt

fi

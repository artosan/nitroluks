SRC_DIR = nitroluks.orig
VERSION = 0.1

init:
	mkdir -p build

clean:
	rm -rf build/
	rm -rf DEBUILD/

binary: init buildlib
	g++ src/nitro_luks.c -o build/nitro_luks -Lbuild/ -l:libnitrokey.so.3 -Wall

buildlib:
	cp -r src/libnitrokey build
	mkdir -p build/libnitrokey/build
	(cd build/libnitrokey/build; cmake ..; make -j2)
	cp build/libnitrokey/build/libnitrokey.so.3 build/

debianize: init
	rm -fr DEBUILD
	mkdir -p DEBUILD/${SRC_DIR}
	cp -r * DEBUILD/${SRC_DIR} || true
	(cd DEBUILD; tar -zcf nitroluks_${VERSION}.orig.tar.gz --exclude=${SRC_DIR}/debian ${SRC_DIR})
	(cd DEBUILD/${SRC_DIR}; debuild -us -uc)

install: binary
	install -D -m 0755 build/nitro_luks $(DESTDIR)/usr/bin/nitro_luks
	install -D -m 0755 build/libnitrokey.so.3 $(DESTDIR)/usr/lib/libnitrokey.so.3
	install -D -m 0755 keyscript.sh $(DESTDIR)/usr/bin/keyscript.sh
	install -D -m 0755 nitroluks_crypttab $(DESTDIR)/etc/nitroluks/nitroluks_crypttab.conf
	install -D -m 0755 initramfs-hook $(DESTDIR)/etc/initramfs-tools/hooks/nitroluks
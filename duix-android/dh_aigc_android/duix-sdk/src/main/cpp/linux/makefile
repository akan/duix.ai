all:
	g++ -fPIC -o gjgpgmain -g \
		-I../third/x86/include/turbojpeg/   \
		gpgmain.cpp \
		../third/x86/lib/libturbojpeg.a \
		../third/x86/lib/libjpeg.a \
		-lm --std=c++11 \
		-DTEST

CC = g++ -std=c++11
INCDIR = -I./  #./libevent/include
LINKDIR = -L./ -L/usr/local/ssl/lib  #libevent/lib
SRCS = *.cpp *.c
LINKLIB = -lpthread -lssl -lcrypto #-levent_core -levent_extra -levent_openssl -levent_pthreads
# -lopencv_imgcodecs
LIBRARY = libonvif.so
all:
	$(CC) $(SRCS) $(INCDIR) -fPIC -shared -DWITH_OPENSSL -O2 -g -o $(LIBRARY) $(LINKDIR) $(LINKLIB)
	cp $(LIBRARY) ./test
	cp $(LIBRARY) ./live555Rtsp
	cp onvif_interface.h ./test
	cp onvif_interface.h ./live555Rtsp
	cp onvif_comm.h     ./test
	cp onvif_comm.h     ./live555Rtsp
	cp soapH.h     ./test
	cp soapH.h     ./live555Rtsp
	cp soapStub.h     ./test
	cp soapStub.h     ./live555Rtsp
	cp stdsoap2.h     ./test
	cp stdsoap2.h     ./live555Rtsp
	cp wsaapi.h     ./test
	cp wsaapi.h     ./live555Rtsp
clean:
	rm -rf libonvif.so


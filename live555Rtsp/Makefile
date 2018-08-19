CC = g++ -std=c++11
INCDIR = -I. -I/usr/local/include -I/usr/local/include/liveMedia -I/usr/local/include/UsageEnvironment -I/usr/local/include/groupsock -I/usr/local/include/BasicUsageEnvironment    #./libevent/include
LINKDIR = -L. -L./usr/lib/x86_64-linux-gnu/ -L/lib/local/ssl/lib # ../libevent/lib -L ./
SRCS = *.cpp
LINKLIB = -lpthread -lssl -lcrypto -lavcodec -lavutil -lavformat -lswscale -lswresample -lopencv_core -lonvif -lliveMedia -lgroupsock -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_imgcodecs -lBasicUsageEnvironment -lUsageEnvironment `pkg-config --libs --cflags opencv` -ldl
LIBRARY = test

all:
	$(CC) $(SRCS) $(INCDIR) -DWITH_OPENSSL -O2  -g -o $(LIBRARY) $(LINKDIR) $(LINKLIB)
clean:
	rm test


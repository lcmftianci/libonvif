CC = g++ -std=c++11
INCDIR = -I.#./libevent/include
LINKDIR = -L. -L./usr/lib/x86_64-linux-gnu/ -L/lib/local/ssl/lib # ../libevent/lib -L ./
SRCS = test.cpp
LINKLIB = -lpthread -lssl -lcrypto -lavcodec -lavutil -lavformat -lswscale -lswresample -lopencv_core -lonvif -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_imgcodecs `pkg-config --libs --cflags opencv` -ldl
LIBRARY = test

all:
	$(CC) $(SRCS) $(INCDIR) -DWITH_OPENSSL -O2  -g -o $(LIBRARY) $(LINKDIR) $(LINKLIB)
	#cp $(LIBRARY) /home/VideoAnalysis/Bin/
clean:
	rm test


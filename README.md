#libonvif
sudo apt-get install libssl-dev
wget http://www.live555.com/liveMedia/public/live555-latest.tar.gz
tar -zxvf live555-latest.tar.gz
cd live555
./genMakefiles  linux-with-shared-libraries
make -j8 install
compile opencv & ffmpeg
cd onvif-dir
make
cd test
make
./test

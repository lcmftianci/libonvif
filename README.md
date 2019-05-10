# libonvif

## 1.install openssl

sudo apt-get install libssl-dev

## 2.install live555

wget http://www.live555.com/liveMedia/public/live555-latest.tar.gz
tar -zxvf live555-latest.tar.gz
cd live555

./genMakefiles  linux-with-shared-libraries

make -j8 install

## 3.compile and install opencv & ffmpeg

compile opencv & ffmpeg

## 4.make onvif library

cd onvif-dir
make

## test onviflib get rtsp video by ffmpeg  and show image by opencv

cd test
make
./test

## test by live555, get rtsp video by live555 and show image by opencv

cd live555Rtsp
make
./test


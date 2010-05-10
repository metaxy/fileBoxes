#build libary
cd libfileboxes
rm -r build -f
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ..
make
make install
#build fileboxes
cd ../../
rm -r build -f
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ..
make
make install



rm -rf build
mkdir build
cd build
cmake ..
make
mv cbpsdb.self eboot.bin
cd ..
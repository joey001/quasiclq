cd %1
cd %2
cmake -G "MinGW Makefiles" ..
mingw32-make
cd ..
cd ..

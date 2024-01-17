echo "Compiling image.cc"
g++ -Wall -O3 -g -std=c++11 -c -o image.o image.cc
echo "Compiling Eva.cc"
g++ -Wall -O3 -g -std=c++11 -c -o Eva.o Eva.cc
echo "Compiling fbplayer.cc"
g++ -Wall -O3 -g -std=c++11 -c -o fbplayer.o fbplayer.cc 
echo "Linking"
g++ -I. -Wall -O3 -g -std=c++11 -o fbplayer fbplayer.o image.o Eva.o -lm -lpthread 
echo "Done"

all:
	unzip data.zip
	g++ src/*.cpp -lSDL2 -lSDL2_Image -o reactop -w

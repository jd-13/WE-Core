CC = g++

default: WECoreTest

catchMain.o: $(BUILD_ROOT)/Tests/catchMain.cpp
	$(CC) -c $(BUILD_ROOT)/Tests/catchMain.cpp -o catchMain.o -I$(CATCH_PATH)

WECoreTest: catchMain.o
	$(CC) catchMain.o -o WECoreTest

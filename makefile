CC = g++

default: WECoreTest

catchMain.o: $(WECORE_SRC)/Tests/catchMain.cpp
	$(CC) -c $(WECORE_SRC)/Tests/catchMain.cpp -o catchMain.o -I$(CATCH_PATH)

WECoreTest: catchMain.o
	$(CC) catchMain.o -o WECoreTest

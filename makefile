CXX = g++

echo $(WECORE_SRC)
echo $(CATCH_PATH)

default: WECoreTest

catchMain.o: $(WECORE_SRC)/Tests/catchMain.cpp
	$(CXX) -c $(WECORE_SRC)/Tests/catchMain.cpp -o catchMain.o -I$(CATCH_PATH)

WECoreTest: catchMain.o
	$(CXX) catchMain.o -o WECoreTest

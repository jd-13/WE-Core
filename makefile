CXXFLAGS = -Wall -Wextra -Wconversion -Wshadow -Wpedantic

default: WECoreTest

catchMain.o: $(WECORE_SRC)/Tests/catchMain.cpp
	$(info $$WECORE_SRC is [${WECORE_SRC}])
	$(info $$CATCH_PATH is [${CATCH_PATH}])
	$(CXX) -c $(WECORE_SRC)/Tests/catchMain.cpp -o catchMain.o -I$(CATCH_PATH) $(CXXFLAGS)

WECoreTest: catchMain.o
	$(CXX) catchMain.o -o WECoreTest

clean:
	rm *.o
	rm WECoreTest

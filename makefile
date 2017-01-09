CXXFLAGS = -std=c++11 -Wall -Werror -Wextra -Wconversion -Wshadow
WECORE_HEADERS = -I$(WECORE_SRC)/General -I$(WECORE_SRC)/Tests -I$(WECORE_SRC)/CarveDSP -I$(WECORE_SRC)/RichterLFO -I$(WECORE_SRC)/SongbirdFilters -I$(WECORE_SRC)/WEFilters

ifeq ($(CXX), clang++)
CXXFLAGS += -Weverything -Wpedantic

# TODO: work on removing the need for the below flags
CXXFLAGS += -Wno-exit-time-destructors -Wno-float-equal -Wno-weak-vtables -Wno-reserved-id-macro -Wno-double-promotion

else
CXXFLAGS += -pedantic
endif

default: WECoreTest

catchMain.o: $(WECORE_SRC)/Tests/catchMain.cpp
	$(CXX) -c $(WECORE_SRC)/Tests/catchMain.cpp -o catchMain.o -I$(CATCH_PATH) $(CXXFLAGS)

CarveDSPUnitTests.o: $(WECORE_SRC)/Tests/CarveDSPUnitTests.cpp
	$(CXX) -c $(WECORE_SRC)/Tests/CarveDSPUnitTests.cpp -o CarveDSPUnitTests.o -I$(CATCH_PATH) $(WECORE_HEADERS) $(CXXFLAGS)

RichterLFOPairTests.o: $(WECORE_SRC)/Tests/RichterLFOPairTests.cpp
	$(CXX) -c $(WECORE_SRC)/Tests/RichterLFOPairTests.cpp -o RichterLFOPairTests.o -I$(CATCH_PATH) $(WECORE_HEADERS) $(CXXFLAGS)

SongbirdFilterModuleTests.o: $(WECORE_SRC)/Tests/SongbirdFilterModuleTests.cpp
	$(CXX) -c $(WECORE_SRC)/Tests/SongbirdFilterModuleTests.cpp -o SongbirdFilterModuleTests.o -I$(CATCH_PATH) $(WECORE_HEADERS) $(CXXFLAGS)


WECoreTest: catchMain.o CarveDSPUnitTests.o RichterLFOPairTests.o SongbirdFilterModuleTests.o
	$(CXX) catchMain.o CarveDSPUnitTests.o RichterLFOPairTests.o SongbirdFilterModuleTests.o -o WECoreTest

clean:
	rm *.o
	rm WECoreTest

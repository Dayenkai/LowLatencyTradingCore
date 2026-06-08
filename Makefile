

main:
	g++ -std=c++23 -pthread -O3 -DNDEBUG -march=native src/main.cc src/EnvStartup/envStartup.cc src/NIC/nicReplay.cc src/PersonnalLibrary/OutputFunctions.cc src/FeedHandler/feedHandler.cc src/Utils/utils.cpp
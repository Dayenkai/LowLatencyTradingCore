

main:
	g++ -std=c++20 -pthread -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer src/main.cc src/EnvStartup/envStartup.cc src/NIC/nicReplay.cc src/PersonnalLibrary/OutputFunctions.cc src/FeedHandler/feedHandler.cc
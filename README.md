command line compilation for windows:
g++ -std=c++17 -o eksamen \
    main.cpp \
    src/Client.cpp src/Server.cpp \
    src/Game/Player.cpp src/Game/State.cpp src/Game/LocalState.cpp \
    -lws2_32 -lmswsock

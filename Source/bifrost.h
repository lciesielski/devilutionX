// bifrost.h
#include <iostream>
#include <winsock2.h>
#include <thread>

namespace devilution {

class bifrost {
public:
	static void startServer(); // Declare the static method to start the server
	static void startServerThread();
};

} // namespace devilution

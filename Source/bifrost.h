// bifrost.h
#include <iostream>
#include <winsock2.h>
#include <thread>

namespace devilution {

class bifrost {
public:
	static void startServer(); // Declare the static method to start the server
	static void startServerThread();

private:
	static std::string handleDataBuffer();
	static std::string handleHealthyBuffer();
	static std::string handleTestBuffer();
	static std::string getLvlDownPosition();
	static std::string getPositionsAroundPlayer();
	static std::string getMonstersOnLevel();
	static std::string getHighlightedMonster();
	static std::string getHighlightedObject();
	static std::string getTheButcher();
};

} // namespace devilution

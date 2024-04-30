#include "bifrost.h"
#include "player.h"
#include "cursor.h"
#include <levels/trigs.h>

namespace devilution {

const int START_PORT = 33005;
const int dx[8] = { -1, 0, 1, -1, 1, -1, 0, 1 }; // Relative x coordinates
const int dy[8] = { 1, 1, 1, 0, 0, -1, -1, -1 }; // Relative y coordinates

const DWORD processId = GetCurrentProcessId();

void bifrost::startServer()
{
	WSADATA wsaData;
	SOCKET server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int currentPort = START_PORT;

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Winsock initialization failed." << std::endl;
		return;
	}

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;

	bool bound = false;
	do {
		address.sin_port = htons(currentPort);

		// Try to bind the socket
		if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
			// Ghetto MessageBox to debug if socket binding failed
			//MessageBox(NULL, "Hello, this is a message box!", "MessageBox Example", MB_OK);
			std::cerr << "Bind failed on port " << currentPort << " with error: " << WSAGetLastError() << std::endl;
			currentPort++; // Increment the port number
		} else {
			//std::string msg = "Successfully bound to port " + std::to_string(currentPort);
			//MessageBox(NULL, msg.c_str(), "MessageBox Example", MB_OK);
			std::cout << "Successfully bound to port " << currentPort << std::endl;
			bound = true;
		}
	} while (!bound);

	if (listen(server_fd, 3) == SOCKET_ERROR) {
		std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(server_fd);
		WSACleanup();
		return;
	}

	while (true) { // Loop to accept multiple connections
		SOCKET new_socket;
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
			std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
			continue; // Continue listening for new connections even if accept fails
		}

		char buffer[255] = { 0 };
		recv(new_socket, buffer, 255, 0);

		std::string response;

		if (strcmp(buffer, "_DATA_") == 0) {
			response = handleDataBuffer();
		} else if (strcmp(buffer, "_HEALTHY_") == 0) {
			response = handleHealthyBuffer();
		} else if (strcmp(buffer, "_TEST_") == 0) {
			response = handleTestBuffer();
		} else {
			response = "{}";
		}

		// Set Content-Type to application/json
		//std::string httpHeader = "HTTP/1.1 200 OK\nContent-Type: application/json\n";
		//httpHeader += "Content-Length: " + std::to_string(response.length()) + "\n\n";

		send(new_socket, response.c_str(), response.length(), 0);
		closesocket(new_socket); // Close the client socket after handling
	}

	// The following code will now never be reached. If you ever want to shut down the server, you'll need to implement a mechanism to break out of the loop.
	// closesocket(server_fd);
	// WSACleanup();
}

void bifrost::startServerThread()
{
	std::thread serverThread(startServer);
	serverThread.detach();
}

std::string bifrost::handleDataBuffer()
{
	const Player &myPlayer = *MyPlayer;
	std::string response = "{\n";
	if (&myPlayer != nullptr) {
		response += "\"levelChanging\": " + std::to_string(myPlayer._pLvlChanging) + ",\n";
		response += "\"experience\": " + std::to_string(myPlayer._pExperience) + ",\n";
		response += "\"level\": " + std::to_string(myPlayer.getCharacterLevel()) + ",\n";
		response += "\"life\": " + std::to_string(myPlayer._pHitPoints >> 6) + ",\n";
		response += "\"mana\": " + std::to_string(myPlayer._pMana >> 6) + ",\n";
		response += "\"dungeonLevel\": " + std::to_string(myPlayer.plrlevel) + ",\n";
		response += "\"position-x\": " + std::to_string(myPlayer.position.tile.x) + ",\n";
		response += "\"position-y\": " + std::to_string(myPlayer.position.tile.y) + ",\n";

		response += getPositionsAroundPlayer();
		response += getLvlDownPosition();
		response += getLvlUpPosition();
		response += getTownShortcutPosition();

		response += getTheButcher();
		response += getHighlightedObject();
		response += getHighlightedMonster();
		response += getMonstersOnLevel();

		response += "\"pauseMode\": " + std::to_string(PauseMode) + ",\n";
		response += "\"processId\": " + std::to_string(processId) + "\n";
	} else {
		response += "\"processId\": " + std::to_string(processId) + "\n";
	}

	response += "}\n";
	return response;
}

std::string bifrost::getLvlDownPosition()
{
	bool descent = false;
	std::string response = "";

	for (int i = 0; i < numtrigs; i++) {
		if (trigs[i]._tmsg == WM_DIABNEXTLVL) {
			response += "\"descent-x\": " + std::to_string(trigs[i].position.x) + ",\n";
			response += "\"descent-y\": " + std::to_string(trigs[i].position.y) + ",\n";
			descent = true;
			break;
		}
	}

	if (!descent)
	{
		response += "\"descent-x\": -1,\n";
		response += "\"descent-y\": -1,\n";
	}

	return response;
}

std::string bifrost::getLvlUpPosition()
{
	bool ascent = false;
	std::string response = "";

	for (int i = 0; i < numtrigs; i++) {
		if (trigs[i]._tmsg == WM_DIABPREVLVL) {
			response += "\"ascent-x\": " + std::to_string(trigs[i].position.x) + ",\n";
			response += "\"ascent-y\": " + std::to_string(trigs[i].position.y) + ",\n";
			ascent = true;
			break;
		}
	}

	if (!ascent)
	{
		response += "\"ascent-x\": -1,\n";
		response += "\"ascent-y\": -1,\n";
	}

	return response;
}

std::string bifrost::getTownShortcutPosition()
{
	bool town = false;
	std::string response = "";

	for (int i = 0; i < numtrigs; i++) {
		if (trigs[i]._tmsg == WM_DIABTWARPUP) {
			response += "\"town-x\": " + std::to_string(trigs[i].position.x) + ",\n";
			response += "\"town-y\": " + std::to_string(trigs[i].position.y) + ",\n";
			town = true;
			break;
		}
	}

	if (!town)
	{
		response += "\"town-x\": -1,\n";
		response += "\"town-y\": -1,\n";
	}

	return response;
}

std::string bifrost::getPositionsAroundPlayer()
{
	const Player &myPlayer = *MyPlayer;
	std::string validPositions = "\"valid-positions\" :[";
	std::string invalidPositions = "\"invalid-positions\" :[";

	for (int i = 0; i < 8; i++) {
		if (PosOkPlayer(myPlayer, { myPlayer.position.tile.x + dx[i], myPlayer.position.tile.y + dy[i] })) {
			validPositions += "{\n";
			validPositions += "\"x\": " + std::to_string(myPlayer.position.tile.x + dx[i]) + ",\n";
			validPositions += "\"y\": " + std::to_string(myPlayer.position.tile.y + dy[i]) + "\n";
			validPositions += "},";
		} else {
			invalidPositions += "{\n";
			invalidPositions += "\"x\": " + std::to_string(myPlayer.position.tile.x + dx[i]) + ",\n";
			invalidPositions += "\"y\": " + std::to_string(myPlayer.position.tile.y + dy[i]) + "\n";
			invalidPositions += "},";
		}
	}

	if (validPositions.back() == ',') {
		validPositions.pop_back();
	}
	validPositions += "],\n";

	if (invalidPositions.back() == ',') {
		invalidPositions.pop_back();
	}
	invalidPositions += "],\n";

	return validPositions.append(invalidPositions);
}

std::string bifrost::getMonstersOnLevel()
{
	std::string monsterPositions = "\"monsters\" :[";

	for (int i = 0; i < MaxMonsters; i++) {
		Monster &monster = Monsters[i];

		if (!monster.isPlayerMinion())
		{
			monsterPositions += "{\n";
			monsterPositions += "\"mon\": " + std::to_string(i) + ",\n";
			monsterPositions += "\"life\": " + std::to_string(monster.hitPoints >> 6) + "\n";
			//monsterPositions += "\"name\": \"" + std::string(monster.name()) + "\",\n";
			//monsterPositions += "\"x\": " + std::to_string(Monsters[i].position.tile.x) + ",\n";
			//monsterPositions += "\"y\": " + std::to_string(Monsters[i].position.tile.y) + "\n";
			monsterPositions += "},";
		}
	}

	if (monsterPositions.back() == ',') {
		monsterPositions.pop_back();
	}

	monsterPositions += "],\n";

	return monsterPositions;
}

std::string bifrost::getHighlightedMonster()
{
	std::string response = "";

	if (pcursmonst != -1) {
		Monster &monster = Monsters[pcursmonst];

		response += "\"target-mon\": " + std::to_string(pcursmonst) + ",\n";
		response += "\"target-mon-health\": " + std::to_string(monster.hitPoints >> 6) + ",\n";
	}
	else
	{
		response += "\"target-mon\": " + std::to_string(pcursmonst) + ",\n";
		response += "\"target-mon-health\": " + std::to_string(pcursmonst) + ",\n";
	}

	return response;
}

std::string bifrost::getHighlightedObject()
{
	std::string response = "";

	if (ObjectUnderCursor != nullptr) {
		const Object &object = *ObjectUnderCursor;
		//response += "\"target-obj\": " + std::to_string(object._otype) + ",\n";
		response += "\"target-obj-door\": " + std::to_string(object.isDoor()) + ",\n";
		//DOOR_OPEN = 1, DOOR_BLOCKED = 2 -> objects.cpp enum
		response += "\"target-obj-door-opened\": " + std::to_string(object._oVar4 == 1 || object._oVar4 == 2) + ",\n";
	}
	else
	{
		//response += "\"target-obj\": -1,\n";
		response += "\"target-obj-door\": -1,\n";
		response += "\"target-obj-door-opened\": -1,\n";
	}

	return response;
}

std::string bifrost::getTheButcher()
{
	std::string response = "";
	const Player &myPlayer = *MyPlayer;

	if (myPlayer.plrlevel == 2 && !myPlayer._pLvlChanging)
	{
		for (int i = 0; i < MaxMonsters; i++) {
			Monster &monster = Monsters[i];

			if (!monster.isPlayerMinion()) {
				if (std::string(monster.name()) == "The Butcher") {
					response += "\"butcher\": " + std::to_string(pcursmonst) + ",\n";
					response += "\"butcher-life\": " + std::to_string(monster.hitPoints >> 6) + ",\n";
					response += "\"butcher-x\": " + std::to_string(Monsters[i].position.tile.x) + ",\n";
					response += "\"butcher-y\": " + std::to_string(Monsters[i].position.tile.y) + ",\n";

					break;
				}
			}
		}
	}
	else
	{
		response += "\"butcher\": -1,\n";
		response += "\"butcher-life\": -1,\n";
		response += "\"butcher-x\": -1,\n";
		response += "\"butcher-y\": -1,\n";
	}

	return response;
}

std::string bifrost::handleHealthyBuffer()
{
	Player &myPlayer = *MyPlayer;
	std::string response = "{\n";
	if (&myPlayer != nullptr) {
		SetPlrVit(myPlayer, 500);
		response += "\"player-vitality\": 500\n";
	} else {
		response += "\"player-vitality\": -1\n";
	}

	response += "}\n";
	return response;
}

std::string bifrost::handleTestBuffer()
{
	std::string response = "{\"test-response\": 200}";
	return response;
}

} // namespace devilution

#include "bifrost.h"
#include "player.h"

namespace devilution {

const int PORT = 33005;

void bifrost::startServer()
{
	WSADATA wsaData;
	SOCKET server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	DWORD processId = GetCurrentProcessId();

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
	address.sin_port = htons(PORT);

	// Bind the socket
	//TODO: Increment socket for every running process
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
		//Ghetto MessageBox to debug if socket binding failed
		//MessageBox(NULL, "Hello, this is a message box!", "MessageBox Example", MB_OK);
		std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
		closesocket(server_fd);
		WSACleanup();
		return;
	}

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

		char buffer[65536] = { 0 };
		recv(new_socket, buffer, 65536, 0);

		std::string response;

		const Player &myPlayer = *MyPlayer;
		if (&myPlayer != nullptr) {
			response = "{\n";
			response += "\"experience\": " + std::to_string(myPlayer._pExperience) + ",\n";
			response += "\"life\": " + std::to_string(myPlayer._pHitPoints >> 6) + ",\n";
			response += "\"mana\": " + std::to_string(myPlayer._pMana >> 6) + ",\n";
			response += "\"dungeonLevel\": " + std::to_string(myPlayer.plrlevel) + ",\n";
			response += "\"position-x\": " + std::to_string(myPlayer.position.tile.x) + ",\n";
			response += "\"position-y\": " + std::to_string(myPlayer.position.tile.y) + ",\n";
			response += "\"pauseMode\": " + std::to_string(PauseMode) + ",\n";
			response += "\"processId\": " + std::to_string(processId) + "\n";
			response += "}\n";
		} else {
			response = "{\n";
			response += "\"processId\": " + std::to_string(processId) + "\n";
			response += "}\n";
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

} // namespace devilution

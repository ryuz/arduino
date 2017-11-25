#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include "SerialSlip.h"


void print_packet(std::string str, std::vector<unsigned char> packet)
{
	std::cout << str;

	std::ios::fmtflags flags = std::cout.flags();
	std::cout << std::hex << std::setfill('0') << std::setw(2);
	for ( auto c : packet) {
		std::cout << (int)c << ' ';
	}
	std::cout.flags(flags);

	std::cout << std::endl;
}

int main()
{
	SerialSlip	ss;

	if (!ss.Open(16, 115200)) {
		return 1;
	}


	for (int i = 0; i < 200; i++) {
		std::vector<unsigned char> send_buf;

		// ランダムなパケットを生成
		int len = rand() % 16 + 1;
		for (int j = 0; j < len; j++) {
			send_buf.push_back((unsigned char)rand());
		}

		auto start_time = std::chrono::system_clock::now();
		ss.SendPacket(send_buf);			// 送信
		auto recv_buf = ss.RecvPacket();	// 受信
		auto end_time = std::chrono::system_clock::now();
		
		print_packet("send:", send_buf);
		print_packet("recv:", recv_buf);
		std::cout << "time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "[ms]" << std::endl;

		if (send_buf != recv_buf) {
			std::cout << "NG" << std::endl;
		}
	}

	ss.Close();

	return 0;
}




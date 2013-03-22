#ifndef _PKLIB_H
#define _PKLIB_H

#include <stdint.h>
#include <vector>
#include <future>

namespace PacketRadio {
	class Packet {
	public:
		uint8_t dst_addr;
		uint8_t src_addr;
		uint8_t control;
		uint8_t length;
		uint8_t txNode;
		uint8_t rxNode;
		std::vector<uint8_t> payload;

		Packet(uint8_t _dst, uint8_t _src, uint8_t _control, std::vector<uint8_t> _payload);
		Packet(const uint8_t * buffer);
	};

	class PacketRadio {
	public:
		PacketRadio(const char * filename, uint8_t address);
		void sendPacket(const Packet& p);
		int getFD();
		std::future<Packet> recvPacket();
	private:
		int fd;

		void setAddress(uint8_t address);
		void openPort(const char * filename);
	};
}

#endif

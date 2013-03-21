#include <pklib.h>
#include <pklib-internal.h>

#include <unistd.h>
#include <types.h>
#include <fcntl.h>
#include <termios.h>

PacketRadio::PacketRadio::PacketRadio(const char * filename, uint8_t address) {
	this->openPort(filename);
	this->setAddress(address);
}

/// Initialise board address
void PacketRadio::PacketRadio::setAddress(uint8_t address) {
	char cmd[2] = { SET_ADDRESS, addr };
	write(fd, cmd, 2);
}

/// Open device filesystem object
void PacketRadio::PacketRadio::openPort(const char * filename) {
	this->fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY); //open in read-write, non-blocking mode

	if (this->fd < 0) {
		perror("open_port: Unable to open device");
		exit(-1);
	} else {
		fcntl(this->fd, F_SETFL, 0);
	}

	//setup RS232 terminal
	struct termios newtio;
	memset(&newtio, 0, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	// set input mode (non-canonical, no echo,...)
	newtio.c_lflag = 0;

	// inter-character timer unused
	newtio.c_cc[VTIME] = 0; 

	// blocking read until header received
	newtio.c_cc[VMIN] = MAX_SEGMENT_SIZE; //modified to allow buffering a whole packet JW 22/3/13 //HEADER_SIZE;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);
}

/// Return internal file descriptor
int PacketRadio::PacketRadio::getFD() {
	return this->fd;
}

/// Return a future object for the next packet to be received
std::future<PacketRadio::Packet> PacketRadio::PacketRadio::recvPacket() {
	return std::async(std::launch::async, [](){
		uint8_t buffer[MAX_SEGMENT_SIZE];

		read(this->fd, buffer, HEADER_SIZE); //test me, depends on the VMIN setting in openPort

		if (buf[1] == 0x7e) { //statistics packet
			read(this->fd, buffer + HEADER_SIZE, 9);
		} else if (buf[LENGTH_BYTE_OFFSET] > 0) { //test to see whether the packet has non-zero length
			int len = buf[LENGTH_BYTE_OFFSET];
			read(this->fd, buffer + HEADER_SIZE, len); //depends on the VMIN setting
		}

		return PacketRadio::Packet(buffer);
	});
}

PacketRadio::Packet::Packet(const uint8_t * buffer) {
	this->dst_addr = buffer[1];
	this->src_addr = buffer[2];
	this->control  = buffer[3];
	this->length   = buffer[4];
	this->txNode   = buffer[5];
	this->rxNode   = buffer[6];

	for (int i = 7; i < this->length + 7; i++) { //check terminator, may be off
		this->payload.push_back(buffer[i]);
	}
}
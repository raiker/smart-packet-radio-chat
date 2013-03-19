#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdlib.h>
#include "pklib.h"

static char rcs_id[] = "$Id: pklib.c,v 1.4 2013/02/18 12:15:14 ahmet Exp $";

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

int open_port(char *dev) {
	int fd; /* File descriptor for the port */

	fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		perror("open_port: Unable to open device");
		exit(-1);
	} else
		fcntl(fd, F_SETFL, 0);

	return (fd);
}

/*
 * Given payload is a string.
 * Return the length of packet.
 */
int create_packet(int dest, int src, char control, const char *payload,
		PacketPtr ptr) {

	ptr->dest = dest;
	ptr->src = src;
	ptr->control = control;
	ptr->rxNode = 0xff;
	ptr->txNode = 0xff;
	ptr->length = strlen(payload);
	strncpy(ptr->payload, payload, PAYLOAD_SIZE);
	return ptr->length;

}

int transmit(PacketPtr packet, int fd) {
	static int counter = 0;
	char *bytes, *current;
	current = calloc(OVERHEAD_SIZE + MAX_SEGMENT_SIZE, sizeof(char));
	bytes = current;
	if (bytes) {
		*(current++) = TRANSMIT;
		*(current++) = packet->dest;
		*(current++) = packet->length + HEADER_SIZE;
		pk2chars(packet, current);
		int n = write(fd, bytes, OVERHEAD_SIZE + MAX_SEGMENT_SIZE);
		if (n < 0) {
			printf("Error when writing to the board.");
			exit(n);
		}
		free(bytes);
		current = bytes = NULL;
	}
	return ++counter;
}

int chars2pk(PacketPtr pk, char *buffer) {
	buffer++;
	pk->dest = *(buffer++);
	pk->src = *(buffer++);
	pk->control = *(buffer++);
	pk->length = *(buffer++);
	pk->txNode = *(buffer++);
	pk->rxNode = *(buffer++);
	strncpy(pk->payload, buffer, pk->length + 1);
	return 1;
}

int is_ack(PacketPtr pk) {
	return (pk->control && 0x90);
}

void setAddress(int fd, int addr) {
	/*
	 * Init board address.
	 */
	char cmd[2] = { SET_ADDRESS, addr };
	write(fd, cmd, 2);

}
void Initialisation(int fd) {
	/*
	 * Enable Aloha mode
	 */
	char cmd2[2] = { MODE,ALOHA_ENABLE };
	write(fd, cmd2, 2);

	char cmd3[2] = { SET_MAX_TRANS,0x6 };
	write(fd, cmd3, 2);

	char cmd4[2] = { SET_SLOT_TIME,0x6 };
	write(fd, cmd4, 2);
}

void pk2chars(PacketPtr packet, char *buf) {
	sprintf(buf, "%c%c%c%c%c%c", packet->dest, packet->src, packet->control,
			packet->length, packet->txNode, packet->rxNode);
	buf += HEADER_SIZE;
	int i;
	for (i = 0; i < PAYLOAD_SIZE; i++) {
		*(buf + i) = *(packet->payload + i);
	}
}

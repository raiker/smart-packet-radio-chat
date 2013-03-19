// $Id: pklib.h,v 1.4 2013/02/18 03:42:12 ahmet Exp $

#ifndef _PKRADIO_H
#define MAX_SEGMENT_SIZE 64
#define HEADER_SIZE 6
#define PAYLOAD_SIZE (MAX_SEGMENT_SIZE - HEADER_SIZE -1)
#define OVERHEAD_SIZE 3
#define LENGTH_BYTE_OFFSET 4
#define ACK_LENGTH 6

/* API Commands */
#define RESET 0x01
#define ALOHA_DISABLE 0x00
#define ALOHA_ENABLE 0x01
#define MODE 0x02
#define SET_ADDRESS 0x08
#define SET_SLOT_TIME 0x40
#define SET_BACKOFF_LIMIT 0x20
#define SET_MAX_TRANS 0x80
#define TRANSMIT 0x10

#define STATUS_NON_INIT 0x0
#define STATUS_IDLE 0x04
#define STATUS_RADIO_READY 0x01
#define STATUS_USER_READY 0x02
#define STATUS_WATING_ACK 0x10

#define SET_STATUS_NON_INIT(x) (x|=STATUS_NON_INIT)
#define SET_STATUS_IDLE(x) (x|=STATUS_IDLE)
#define SET_STATUS_RADIO_READY(x) (x|=STATUS_RADIO_READY)
#define SET_STATUS_USER_READY(x) (x|=STATUS_USER_READY)
#define SET_STATUS_WATING_ACK(x) (x|=STATUS_WATING_ACK)

#define UNSET_STATUS_NON_INIT(x) (x&= ~STATUS_NON_INIT)
#define UNSET_STATUS_IDLE(x) (x&= ~STATUS_IDLE)
#define UNSET_STATUS_RADIO_READY(x) (x&= ~STATUS_RADIO_READY)
#define UNSET_STATUS_USER_READY(x) (x&= ~STATUS_USER_READY)
#define UNSET_STATUS_WATING_ACK(x) (x&= ~STATUS_WATING_ACK)

#define IS_STATUS_NON_INIT(x) (x&STATUS_NON_INIT)
#define IS_STATUS_IDLE(x) (x&STATUS_IDLE)
#define IS_STATUS_RADIO_READY(x) (x&STATUS_RADIO_READY)
#define IS_STATUS_USER_READY(x) (x&STATUS_USER_READY)
#define IS_STATUS_WATING_ACK(x) (x&STATUS_WATING_ACK)

#define BAUDRATE B115200

#ifdef DEBUG
#define PRINT_PK(packet) printf("DEST: %d\tSRC: %d\tCRTL: %X\t LENGTH: %d\nTX: %d\tRX: %d\tPayload: %s\n ", \
		packet->dest, packet->src, packet->control, packet->length, packet->txNode, packet->rxNode, packet->payload)
#else
#define PRINT_PK(packet) printf("FROM %d\t: %s\n ", packet->src, packet->payload)
#endif

#ifdef DEBUG
#define PRINT_ACK(packet) printf("ACK received.\t")
#else
#define PRINT_ACK(packet)
#endif

typedef struct _packet {
	unsigned char src;
	unsigned char dest;
	unsigned char control;
	unsigned char length;
	unsigned char txNode;
	unsigned char rxNode;
	char payload[PAYLOAD_SIZE];
} Packet;
typedef Packet *PacketPtr;
typedef unsigned char Status;

int open_port(char *dev);
int transmit(PacketPtr packet, int fd);
int chars2pk(PacketPtr pk, char *buffer);
int create_packet(int dest, int src, char control, const char *payload,
		PacketPtr);
int is_ack(PacketPtr);
void setAddress(int fd, int addr);
void pk2chars(PacketPtr packet, char *buffer);

#endif


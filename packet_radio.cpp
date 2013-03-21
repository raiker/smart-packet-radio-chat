//packet_radio C++ rewrite
//Joshua Weberruss, Monash University

#include <thread>
#include <condition_variable>
#include <queue>
#include <vector>
#include <string>
#include <sstream>

#include <unistd.h>
#include <termios.h>

#include "pklib.h"

static void set_termino(int fd);

int main(int argc, char **argv) {
	int dest, dev_num;
	int fd;
	//parse commandline arguments
    while ((c = getopt(argc, argv, "d:a:s::")) != -1) {
		switch (c) {
			case 'a':
				//address
				dev_num = strtol(optarg, NULL, 10);
				break;
			case 'd':
				//destination
				dest = strtol(optarg, NULL, 10);
				break;
			case 's':
				sflag = 0;
				break;
			case '?':
				if (optopt == 'a' || optopt == 'd')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			default:
				abort();
				break;
		}
    }
    
    //open a file descriptor to the device
    std::stringbuf dev;
    dev << "/dev/radio" << dest;    
    fd = open_port(dev.c_str);
    
    //something to do with the terminal library
    set_termino(fd);
    
    //set the device address
    setAddress(fd, dev_num);
    Initialisation(fd); //this needs to be OOified

    //packet receipt queue
    std::queue<std::vector<unsigned char>> packet_queue();
    
    //mutex
    std::mutex queue_mutex();

    //receiver signal variable
    std::condition_variable queue_var();

    //create receiver thread
    std::thread receiver_thread([&]() {
    	
    	std::lock_guard<std::mutex> lock(queue_mutex); //RAII
    	packet_queue.push()
    });
}

void set_termino(int fd) {
    struct termios newtio;
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    /* inter-character timer unused */
    newtio.c_cc[VTIME] = 0; 

    /* blocking read until header received */
    newtio.c_cc[VMIN] = HEADER_SIZE; 
    
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
}

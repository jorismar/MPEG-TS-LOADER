#include <iostream>
#include <string>
#include "utils.h"
#include "ts.h"

#define TS_PACKET_SIZE 188

int main(int argc, char *argv[]) {
	std::string filename;
	
	// Select TS file
	if(argc > 1)
		filename = argv[1];
	else
		filename = "video.ts";
		
	// Buffer to store the file data.
	byte_t * buffer;

	// Read file
	unsigned int size = readFile(filename, &buffer);

	CHECK_ERR(size <= 0, "File read error.", 1);
	
	for (unsigned int i = 0; i < size; i += TS_PACKET_SIZE) {
		MPEGTS ts;

		std::cout << "PACKET #" << i/TS_PACKET_SIZE << std::endl;
		// Read packet information
		CHECK_ERR(!readTSpacket(&ts, &buffer[i]), "TS packet read error.", 1);

		// Print packet information
		printTSInfo(&ts);

		std::cout << std::endl << "Press ENTER to read next packet..." << std::endl;
		getchar();
	}

    return 0;
}

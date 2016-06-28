#include "utils.h"

//**************************************************************************************************

int readFile(std::string path, byte_t ** buffer) {
	std::streampos size;

	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	// Check if file exists
	CHECK_ERR(file.fail(), "The requested file " + path + " not found.", -1);

	// Open file
	CHECK_ERR(!file.is_open(), "Unable to open the file.", -1);

	// Gets file size
	size = file.tellg();
	file.seekg(0, std::ios::beg);

	// Allocates the memory space to buffer
	*buffer = new byte_t[size];

	// Read the file
	file.read((char*) *buffer, size);

	file.close();

	return size;
}

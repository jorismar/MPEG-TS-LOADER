/**
* \file 	ts.h
*
* \author 	Jorismar Barbosa Meira <jorismar.barbosa@lavid.ufpb.br>
*/

#ifndef JB_TS_H
#define JB_TS_H

#include <map>
#include <vector>
#include <string>
#include "utils.h"

/**
* \brief This header implements the essential functions and structures for Transport Stream information.
*
* \headerfile ts.h
*/

//******************************* PMT *******************************

/** Defines a new struct to storage the Descriptors informations. */
struct Descriptor {
	// Not Implemented
};

/** Defines a new struct to storage the Elementary Stream informations. */
struct ElementaryStreamInfo {
	byte_t stream_type;					// Stream Type (8 bits)
	// Reserved (3 bits)
	word_t elementary_PID;				// Elementary Stream PID (13 bits)
	// Reserved (4 bits)
	word_t es_info_length;				// Elementary Stream information Length (12 bits)
	Descriptor * descriptor;			// Descriptor (Not Implemented)
};

/** Defines a new struct to storage the the Program Map Table information. */
struct PMT {
	byte_t table_id;					// Table ID (8 bits)
	byte_t section_syntax_indicator;	// Section Syntax Indicator (1 bit)
	// '0' (1 bit)
	// Reserved (2 bits)
	word_t section_length;				// Section Length (12 bits)
	word_t program_number;				// Program Number (16 bits)
	// Reserved (2 bits)
	byte_t version_number;				// Version Number (5 bits)
	byte_t current_next_indicator;		// Current Next Indicator (1 bit)
	byte_t section_number;				// Section Number (8 bits)
	byte_t last_section_number;			// Last Section Number (8 bits)
	// Reserved (3 bits)
	word_t pcr_pid;						// PCR PID (13 bits)
	// Reserved (4 bits)
	word_t program_info_length;			// Program Info Length (12 bits)
	Descriptor * descriptors;			// Descriptors List (Not implemented)
	ElementaryStreamInfo * es_info;		// Elementary Stream list (Variable) 

	dword_t crc;						// CRC 32 (32 bits)
	size_t num_es_info;					// Elementary Stream list size (Not part of the header)
};

//******************************* PAT *******************************

/** Defines a new struct to storage the Program PID information. */
union ProgramPID {
	word_t network_PID;					// Network PID  (13 bits)
	word_t program_map_PID;				// Program Map PID  (13 bits)
};

/** Defines a new struct to storage the Programs informations. */
struct Program {
	word_t program_number;				// Program Number (16 bits)
	// Reserved (3 bits)
	ProgramPID pid;						// Program PID (13 bits)
};

/** Defines a new struct to storage the Program Association Table informations. */
struct PAT {
	byte_t table_id;					// Table ID (8 bits)
	byte_t section_syntax_indicator;	// Section Syntax Indicator (1 bit)
	// '0'
	// Reserved (2 bits)
	word_t section_length;				// Section Length (12 bits)
	word_t transport_stream_id;			// Transport Stream ID (16 bits)
	// Reserved (2 bits)
	byte_t version_number;				// Version Number (5 bits)
	byte_t current_next_indicator;		// Current Next Indicator (1 bit)
	byte_t section_number;				// Section Number (8 bits)
	byte_t last_section_number;			// Last Section Number (8 bits)
	Program * programs;					// Programs List (32 bits * N)
	dword_t crc;						// CRC 32 (32 bits)
	size_t num_programs;				// Programs list size (Not part of the header)
};

//*************************** ADAPTATION ****************************

/** Defines a new struct to storage the Splice Type informations. */
struct SpliceTypeField {
	byte_t type;						// Splice Type (8 bits)
	byte_t dts_next_au1;				// Decoding Time Stamp Next Access Unit (3 bits)
	// Marker Bit (1 bit)
	word_t dts_next_au2;				// Decoding Time Stamp Next Access Unit (15 bits)
	// Marker Bit (1 bit)
	word_t dts_next_au3;				// Decoding Time Stamp Next Access Unit (15 bits)
	// Marker Bit (1 bit)
};

/** Defines a new struct to storage the Plecewise Rate information. */
struct PlecewiseRateField {
	// Reserved (2 bits)
	dword_t rate;						// Plecewise Rate (22 bits)
};

/** Defines a new struct to storage the the LTW information. */
struct LTW {
	byte_t valid_flag;					// LTW Valid Flag (1 bit)
	word_t offset;						// LTW Offset (15 bits)
};

/** Defines a new struct to storage the Adaptation Extension Field information. */
struct AdaptationExtensionField {
	byte_t length;						// Adaptation Field Extension Length (8 bits)
	byte_t ltw_flag;					// LTW Flag (1 bit)
	byte_t plecewise_flag;				// Plecewise Flag (1 bit)
	byte_t seaml_splice_flag;			// Seamless Splice Flag (1 bit)
	// Reserved (5 bits)
	LTW ltw_field;						// LTW Field (Optional)
	PlecewiseRateField plecewise_field;	// Plecewise Rate Field (Optional)
	SpliceTypeField splice_type_field;	// Splice Type Field (Optional)
};

/** Defines a new struct to storage the Transport Private Field information. */
struct TransportPrivateField {
	byte_t length;						// Transport Private Data Length (8 bits)
	byte_t * data;						// Private Data Byte (Variable)
};

/** Defines a new struct to storage the Program Clock Reference information. */
struct PCRField {
	qword_t progrm_clock_ref;			// Program Clock Reference Base (33 bits)
	// Reserved (6 bits)
	word_t progrm_clock_ref_extnsion;	// Program Clock Reference Extension (9 bits)
};

/** Defines a new struct to storage the Adaptation Field information. */
struct AdaptationField {
	byte_t length;						// Adaptation Field Length ( bits)
	byte_t discontinuity_flag;			// Discontinuity Indicator (1 bit)
	byte_t random_access_flag;			// Random Access Indicator (1 bit)
	byte_t elemnt_stream_priority_flag;	// Elementary Stream Priority Indicator (1 bit)
	byte_t pcr_flag;					// PCR Flag (1 bit)
	byte_t opcr_flag;					// OPCR Flag (1 bit)
	byte_t splicing_point_flag;			// Splicing Point Flag (1 bit)
	byte_t transp_pvt_data_flag;		// Transport Private Data Flag (1 bit)
	byte_t adapt_extnsion_flag;			// Adaptation Field Extension Flag (1 bit)
	PCRField pcr_field;					// PCR Field (48 bits)
	PCRField opcr_field;				// OPCR Field (48 bits)
	char splice_countdown;				// Splice Countdown  (8 bits)
	TransportPrivateField transp_pvt_data_field;	// Transport Private Data Field (Variable)
	AdaptationExtensionField adapt_extnsion_field;	// Adaptation Extension Field (Variable)
	// Stuffing Bytes (Variable)
};

//************************ TRANSPORT STREAM *************************

/** Defines a new struct to storage the Program Specific Information. */
union PSI {
	PAT pat;							// Program Association Table
	PMT pmt;							// Program Map Table
};

/** Defines a new struct to storage the Packetized Elementary Stream information. */
struct PES {
	byte_t * data;						// PES Header + Data
};

/** Defines a new struct to storage the Payload information. */
union Payload {
	PSI psi;							// Program Specific information
	PES pes;							// Packetized Elementary Stream
};

/** Defines a new struct to storage the Transport Stream informations. */
struct MPEGTS {
	byte_t sync;						// Sync byte (8 bits)
	byte_t transp_err_flag;				// Transport Error Indicator (1 bit)
	byte_t payl_unit_start_flag;		// Payload Unit Start Indicator (1 bit)
	byte_t transp_priority_flag;		// Transport Priority (1 bit)
	word_t pid;							// PID (13 bits)
	byte_t transp_scramb_control;		// Transport Scrambling Control (2 bits)
	byte_t adapt_field_control;			// Adaptation Field Control (2 bits)
	byte_t continuity_counter;			// Continuity Counter (4 bits)
	AdaptationField adaptation_field;	// Adaptation Field - Optional
	Payload payload;					// Payload - Optional
};

/**
 * \brief	Loads a TS packet and puts your information on a MPEG-TS struct.
 *
 * \param	filename	Path of file
 * \return	Returns a struct of Mpeg-TS information
 */
bool readTSpacket(MPEGTS * packet, byte_t * data);

/**
 * \brief	Print the information of a MPEG-TS struct.
 *
 * \param	packet	A MPEG-TS struct.
 */
void printTSInfo(MPEGTS * packet);

#endif // !JB_TS_H

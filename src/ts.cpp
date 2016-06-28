#include "ts.h"

// Used to storage of the expected PID on the next packet.
std::map<unsigned short, unsigned short> g_expctd_pids;

//****************************************************************************************************

bool readTSpacket(MPEGTS * ts, byte_t * data) {
	// Consistency check.
	CHECK_ERR(ts == NULL, "MPEGTS struct can not be NULL.", false);
	CHECK_ERR(data == NULL, "Data buffer can not be NULL.", false);
	CHECK_ERR((char)data[0] != 'G', "This not is a MPEG-TS ts.", false);

	byte_t aux;				// Auxiliary variable for bit shift operations.
	unsigned int i = 0;		// Reader position indicator.

	// ----------------- Header - Packet Main Field -----------------
	ts->sync = data[i++];

	ts->transp_err_flag	= data[i] >> 7;

	aux = data[i] << 1;
	ts->payl_unit_start_flag = aux >> 7;

	aux = data[i] << 2;
	ts->transp_priority_flag = aux >> 7;

	aux = data[i++] << 3;
	ts->pid = ((aux >> 3) * 256) + data[i++];

	ts->transp_scramb_control = data[i] >> 6;

	aux = data[i] << 2;
	ts->adapt_field_control = aux >> 6;

	aux = data[i++] << 4;
	ts->continuity_counter = aux >> 4;

	// ---------------------- Adaptation Field ----------------------
	if (ts->adapt_field_control == 2 || ts->adapt_field_control == 3) {
		ts->adaptation_field.length = data[i++];

		if (ts->adaptation_field.length > 0) {
			unsigned int count = i;

			ts->adaptation_field.discontinuity_flag = data[i] >> 7;

			aux = data[i] << 1;
			ts->adaptation_field.random_access_flag = aux >> 7;

			aux = data[i] << 2;
			ts->adaptation_field.elemnt_stream_priority_flag = aux >> 7;

			aux = data[i] << 3;
			ts->adaptation_field.pcr_flag = aux >> 7;

			aux = data[i] << 4;
			ts->adaptation_field.opcr_flag = aux >> 7;

			aux = data[i] << 5;
			ts->adaptation_field.splicing_point_flag = aux >> 7;

			aux = data[i] << 6;
			ts->adaptation_field.transp_pvt_data_flag = aux >> 7;

			aux = data[i++] << 7;
			ts->adaptation_field.adapt_extnsion_flag = aux >> 7;

			// -------------------- PCR Field (Optional) --------------------
			if (ts->adaptation_field.pcr_flag) {
				ts->adaptation_field.pcr_field.progrm_clock_ref = (((((((data[i] * 256) + data[i + 1]) * 256) + data[i + 2]) * 256) + data[i + 3]) * 256) + (data[i + 4] >> 7);

				i += 4;	// Last byte is needed to next field

				aux = data[i++] >> 7;
				ts->adaptation_field.pcr_field.progrm_clock_ref_extnsion = (aux * 256) + data[i++];
			}

			// -------------------- OPCR Field (Optional) -------------------
			if (ts->adaptation_field.opcr_flag) {
				ts->adaptation_field.opcr_field.progrm_clock_ref = (((((((data[i] * 256) + data[i + 1]) * 256) + data[i + 2]) * 256) + data[i + 3]) * 256) + (data[i + 4] >> 7);

				i += 4;

				aux = data[i++] >> 7;
				ts->adaptation_field.opcr_field.progrm_clock_ref_extnsion = (aux * 256) + data[i++];
			}

			// -------------- Splice Countdown Field (Optional) --------------
			if (ts->adaptation_field.splicing_point_flag) {
				ts->adaptation_field.splice_countdown = data[i++];
			}

			// ---------- Transport Private Data Field (Optional) -----------
			if (ts->adaptation_field.transp_pvt_data_flag) {
				ts->adaptation_field.transp_pvt_data_field.length = data[i++];
				ts->adaptation_field.transp_pvt_data_field.data = new byte_t[ts->adaptation_field.transp_pvt_data_field.length];

				for (unsigned int p = 0; p < ts->adaptation_field.transp_pvt_data_field.length; p++)
					ts->adaptation_field.transp_pvt_data_field.data[p] = data[i++];
			}

			// ----------- Adaptation Extension Field (Optional) ------------
			if (ts->adaptation_field.adapt_extnsion_flag) {
				ts->adaptation_field.adapt_extnsion_field.length = data[i++];
				ts->adaptation_field.adapt_extnsion_field.ltw_flag = data[i] >> 7;

				aux = data[i] << 1;
				ts->adaptation_field.adapt_extnsion_field.plecewise_flag = data[i] >> 7;

				aux = data[i] << 2;
				ts->adaptation_field.adapt_extnsion_field.seaml_splice_flag = data[i++] >> 7;

				// --- Adaptation Extension Field (Optional) / LTW Field (Optional) ---
				if (ts->adaptation_field.adapt_extnsion_field.ltw_flag) {
					ts->adaptation_field.adapt_extnsion_field.ltw_field.valid_flag = data[i] >> 7;

					aux = data[i++] << 1;
					ts->adaptation_field.adapt_extnsion_field.ltw_field.offset = ((aux >> 1) * 256) + data[i++];
				}

				// --- Adaptation Extension Field (Optional) / Plecewise Rate Field (Optional) ---
				if (ts->adaptation_field.adapt_extnsion_field.plecewise_flag) {
					aux = data[i++] << 2;
					ts->adaptation_field.adapt_extnsion_field.plecewise_field.rate = ((((aux >> 2) * 256) + data[i]) * 256) + data[i + 1];
					i += 2;
				}

				// --- Adaptation Extension Field (Optional) / Splice Type Field (Optional) ---
				if (ts->adaptation_field.adapt_extnsion_field.seaml_splice_flag) {
					ts->adaptation_field.adapt_extnsion_field.splice_type_field.type = data[i] >> 4;

					aux = data[i++] << 4;
					ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au1 = aux >> 5;

					ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au2 = (data[i] * 256) + data[i + 1];
					ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au2 = ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au2 >> 1;
					i += 2;

					ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au3 = (data[i] * 256) + data[i + 1];
					ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au3 = ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au3 >> 1;
					i += 2;
				}
			}

			// ------------------ Ignoring Stuffing Bytes ------------------
			i += ts->adaptation_field.length - (i - count);
		}
	}

	if (ts->adapt_field_control == 1 || ts->adapt_field_control == 3) {
		// Pointer Field
		if (ts->payl_unit_start_flag)
			i += 1 + data[i];	// Increment position and get the pointer field value

		// Program Association Table Field
		if (ts->pid == 0) {
			ts->payload.psi.pat.table_id = data[i++];

			ts->payload.psi.pat.section_syntax_indicator = data[i] >> 7;
			
			aux = data[i++] << 4;
			ts->payload.psi.pat.section_length = ((aux >> 4) * 256) + data[i++];

			aux = data[i++] * 256;
			ts->payload.psi.pat.transport_stream_id = aux + data[i++];

			aux = data[i] << 2;
			ts->payload.psi.pat.version_number = (aux >> 3);

			aux = data[i++] << 7;
			ts->payload.psi.pat.current_next_indicator = aux >> 7;

			ts->payload.psi.pat.section_number = data[i++];

			ts->payload.psi.pat.last_section_number = data[i++];

			ts->payload.psi.pat.num_programs = (ts->payload.psi.pat.section_length - 9) / 4;

			ts->payload.psi.pat.programs = new Program[ts->payload.psi.pat.num_programs];

			for (unsigned int p = 0; p < ts->payload.psi.pat.num_programs; p++) {
				aux = data[i++];
				ts->payload.psi.pat.programs[p].program_number = (aux * 256) + data[i++];

				aux = data[i++] << 3;
				unsigned short pid = ((aux >> 3) * 256) + data[i++];

				g_expctd_pids[pid] = ts->payload.psi.pat.programs[p].program_number;

				if (ts->payload.psi.pat.programs[p].program_number == 0)
					ts->payload.psi.pat.programs[p].pid.network_PID = pid;
				else 
					ts->payload.psi.pat.programs[p].pid.program_map_PID = pid;
			}

			ts->payload.psi.pat.crc = (((((data[i] * 256) + data[i+1]) * 256) + data[i+2]) * 256) + data[i+3];

			i += 4;
		}
		else if (ts->pid == 1) {
			// 	Conditional Access Table (CAT) - Not Implemented
		}
		else if (g_expctd_pids.find(ts->pid) != g_expctd_pids.end()) {
			// Program Map Table Field
			if (g_expctd_pids[ts->pid] != 0) { // Check program number
				ts->payload.psi.pmt.table_id = data[i++];
				ts->payload.psi.pmt.section_syntax_indicator = data[i] >> 7;

				aux = data[i++] << 4;
				ts->payload.psi.pmt.section_length = ((aux >> 4) * 256) + data[i++];

				aux = data[i++];
				ts->payload.psi.pmt.program_number = (aux * 256) + data[i++];

				aux = data[i] << 2;
				ts->payload.psi.pmt.version_number = (aux >> 3);

				aux = data[i++] << 7;
				ts->payload.psi.pmt.current_next_indicator = aux >> 7;

				ts->payload.psi.pmt.section_number = data[i++];

				ts->payload.psi.pmt.last_section_number = data[i++];

				aux = data[i++] << 3;
				ts->payload.psi.pmt.pcr_pid = ((aux >> 3) * 256) + data[i++];

				aux = data[i++] << 4;
				ts->payload.psi.pmt.program_info_length = ((aux >> 4) * 256) + data[i++];

				for (unsigned int p = 0; p < ts->payload.psi.pmt.program_info_length; p++) {
					// ------------- Descriptor - Not Implemented -------------
				}

				i += ts->payload.psi.pmt.program_info_length;

				std::vector<ElementaryStreamInfo> es_information;

				do {
					ElementaryStreamInfo es;

					es.stream_type = data[i++];

					aux = data[i++] << 3;
					es.elementary_PID = ((aux >> 3) * 256) + data[i++];

					aux = data[i++] << 4;
					es.es_info_length = ((aux >> 4) * 256) + data[i++];

					for (unsigned int p = 0; p < es.es_info_length; p++) {
						// ------------- Descriptor - Not Implemented -------------
					}

					i += es.es_info_length;

					es_information.push_back(es);
				} while (ts->payload.psi.pmt.section_length - (i - 3) > 4);

				ts->payload.psi.pmt.num_es_info = es_information.size();
				ts->payload.psi.pmt.es_info = new ElementaryStreamInfo[ts->payload.psi.pmt.num_es_info];

				for (unsigned int p = 0; p < ts->payload.psi.pmt.num_es_info; p++)
					ts->payload.psi.pmt.es_info[p] = es_information.at(p);

				ts->payload.psi.pmt.crc = (((((data[i] * 256) + data[i + 1]) * 256) + data[i + 2]) * 256) + data[i + 3];
			}
			else {
				// ------------- Network Information Table (NIT) - Not Implemented -------------
			}
		}
		else if (ts->pid == 8191) {
			// ------------- Null Packet -------------
		}
	}

	return true;
}

//****************************************************************************************************

void printTSInfo(MPEGTS * ts) {
	bool hasPayload = ts->adapt_field_control == 1 || ts->adapt_field_control == 3;
	bool hasAdaptation = ts->adapt_field_control == 2 || ts->adapt_field_control == 3;

	// Header
	std::cout << "---------------------------------------------------------" << std::endl << " MPEG-TS Packet Information" << std::endl << "---------------------------------------------------------" << std::endl << \
		"Sync byte: "					<< (char)ts->sync << std::endl << \
		"Transport Error Indicator: "	<< (ts->transp_err_flag ? "Yes" : "No") << std::endl << \
		"Payload Unit Start Indicator: "<< (ts->payl_unit_start_flag ? "Yes" : "No") << std::endl << \
		"Transport Priority: "			<< (ts->transp_priority_flag ? "Yes" : "No") << std::endl << \
		"PID: "							<< ts->pid << std::endl << \
		"Scrambling control: "			<< (ts->transp_scramb_control == 2 ? "Even Key" : ts->transp_scramb_control == 3 ? "ODD Key" : "Not scrambled") << std::endl << \
		"Adaptation field control: "	<< (hasAdaptation && hasPayload ? "Adaptation Field Followed by Payload" : hasAdaptation && !hasPayload ? "Adaptation Field Only" : !hasAdaptation && hasPayload ? "Payload only" : "Reserved") << std::endl << \
		"Continuity counter: "			<< (short)ts->continuity_counter << std::endl;

	// Adaptation Field
	if (hasAdaptation) {
		std::cout << std::endl;
		std::cout << "Adaptation Field" << std::endl << "------------------------------------------------" << std::endl << \
			"Adaptation Field Length: " << (short)ts->adaptation_field.length << std::endl << \
			"Discontinuity Indicator: " << (ts->adaptation_field.discontinuity_flag ? "Yes" : "No") << std::endl << \
			"Random Access Indicator: " << (ts->adaptation_field.random_access_flag ? "Yes" : "No") << std::endl << \
			"Elementary Stream Priority Indicator: " << (ts->adaptation_field.elemnt_stream_priority_flag ? "Yes" : "No") << std::endl << \
			"PCR Flag: " << (ts->adaptation_field.pcr_flag ? "Yes" : "No") << std::endl << \
			"OPCR Flag: " << (ts->adaptation_field.opcr_flag ? "Yes" : "No") << std::endl << \
			"Splicing Point Flag: " << (ts->adaptation_field.splicing_point_flag ? "Yes" : "No") << std::endl << \
			"Transport Private Data Flag: " << (ts->adaptation_field.transp_pvt_data_flag ? "Yes" : "No") << std::endl << \
			"Adaptation Field Extension Flag: " << (ts->adaptation_field.adapt_extnsion_flag ? "Yes" : "No") << std::endl;

		// PCR Field
		if (ts->adaptation_field.pcr_flag) {
			std::cout << "PCR Field" << std::endl << \
				"\tProgram Clock Reference Base: " << ts->adaptation_field.pcr_field.progrm_clock_ref << std::endl << \
				"\tProgram Clock Reference Extension: " << ts->adaptation_field.pcr_field.progrm_clock_ref_extnsion << std::endl;
		}

		// OPCR Field
		if (ts->adaptation_field.opcr_flag) {
			std::cout << "OPCR Field" << std::endl << \
				"\tProgram Clock Reference Base: " << ts->adaptation_field.opcr_field.progrm_clock_ref << std::endl << \
				"\tProgram Clock Reference Extension: " << ts->adaptation_field.opcr_field.progrm_clock_ref_extnsion << std::endl;
		}

		// Splice Countdown Field
		if (ts->adaptation_field.splicing_point_flag) {
			std::cout << "Splice Countdown Field" << std::endl << \
				"\tSplice Countdown: " << (short)ts->adaptation_field.splice_countdown << std::endl;
		}

		// Transport Private Data Field
		if (ts->adaptation_field.transp_pvt_data_flag) {
			std::cout << "Transport Private Data Field" << std::endl << \
				"\tPrivate Data Length" << (short)ts->adaptation_field.transp_pvt_data_field.length << std::endl;
		}

		// Adaptation Field Extension
		if (ts->adaptation_field.adapt_extnsion_flag) {
			std::cout << "Adaptation Field Extension" << std::endl << \
				"\tAdaptation Field Extension Length: " << (short)ts->adaptation_field.adapt_extnsion_field.length << std::endl << \
				"\tLTW Flag: " << (ts->adaptation_field.adapt_extnsion_field.ltw_flag ? "Yes" : "No") << std::endl << \
				"\tPlecewise Flag: " << (ts->adaptation_field.adapt_extnsion_field.plecewise_flag ? "Yes" : "No") << std::endl << \
				"\tSeamless Splice Flag: " << (ts->adaptation_field.adapt_extnsion_field.seaml_splice_flag ? "Yes" : "No") << std::endl;
			
			// LTW Field
			if (ts->adaptation_field.adapt_extnsion_field.ltw_flag) {
				std::cout << "\tLTW Field" << std::endl << \
					"\t\tValid Flag: " << (ts->adaptation_field.adapt_extnsion_field.ltw_field.valid_flag ? "Yes" : "No") << std::endl << \
					"\t\tOffset: " << ts->adaptation_field.adapt_extnsion_field.ltw_field.offset << std::endl;
			}

			// Plecewise Rate Field
			if (ts->adaptation_field.adapt_extnsion_field.plecewise_flag) {
				std::cout << "\tPlecewise Rate Field" << std::endl << \
					"\t\tRate: " << ts->adaptation_field.adapt_extnsion_field.plecewise_field.rate << std::endl;
			}

			// Splice Type Field
			if (ts->adaptation_field.adapt_extnsion_field.seaml_splice_flag) {
				std::cout << "\tSplice Type Field" << std::endl << \
					"\t\tType: " << (short)ts->adaptation_field.adapt_extnsion_field.splice_type_field.type << std::endl << \
					"\t\tDecoding Time Stamp Next Access Unit: " << \
					(short)ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au1 << ":" << \
					(short)ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au2 << ":" << \
					(short)ts->adaptation_field.adapt_extnsion_field.splice_type_field.dts_next_au3 << ":" << std::endl;
			}
		}
	}

	if (hasPayload) {
		std::cout << std::endl;
		
		// Program Association Table (PAT)
		if (ts->pid == 0) {
			std::cout << "Program Association Table (PAT)" << std::endl << "------------------------------------------------" << std::endl << \
				"Table ID: " << (short)ts->payload.psi.pat.table_id << std::endl << \
				"Section Syntax Indicator: " << (short)ts->payload.psi.pat.section_syntax_indicator << std::endl << \
				"Section Length: " << ts->payload.psi.pat.section_length << std::endl << \
				"Transport Stream ID: " << ts->payload.psi.pat.transport_stream_id << std::endl << \
				"Version Number: " << (short)ts->payload.psi.pat.version_number << std::endl << \
				"Current Next Indicator: " << (short)ts->payload.psi.pat.current_next_indicator << std::endl << \
				"Section Number: " << (short)ts->payload.psi.pat.section_number << std::endl << \
				"Last Section Number: " << (short)ts->payload.psi.pat.last_section_number << std::endl;

			for (unsigned int i = 0; i < ts->payload.psi.pat.num_programs; i++) {
				std::cout << "Program Number: " << ts->payload.psi.pat.programs[i].program_number << std::endl;

				if (ts->payload.psi.pat.programs[i].program_number == 0)
					std::cout << "Network PID: " << ts->payload.psi.pat.programs[i].pid.network_PID << std::endl;
				else
					std::cout << "Program Map PID: " << ts->payload.psi.pat.programs[i].pid.program_map_PID << std::endl;
			}

			std::cout << "CRC: " << ts->payload.psi.pat.crc << std::endl;
		}

		// Program Map Table (PMT)
		else if (g_expctd_pids.find(ts->pid) != g_expctd_pids.end()) {
			if (g_expctd_pids[ts->pid] != 0) {
				std::cout << "Program Map Table (PMT)" << std::endl << "------------------------------------------------" << std::endl << \
					"Table ID: " << (short)ts->payload.psi.pmt.table_id << std::endl << \
					"Section Syntax Indicator: " << (short)ts->payload.psi.pmt.section_syntax_indicator << std::endl << \
					"Section Length: " << ts->payload.psi.pmt.section_length << std::endl << \
					"Program Number: " << ts->payload.psi.pmt.program_number << std::endl << \
					"Version Number: " << (short)ts->payload.psi.pmt.version_number << std::endl << \
					"Current Next Indicator : " << (short)ts->payload.psi.pmt.current_next_indicator << std::endl << \
					"Section Number: " << (short)ts->payload.psi.pmt.section_number << std::endl << \
					"Last Section Number: " << (short)ts->payload.psi.pmt.last_section_number << std::endl << \
					"PCR PID: " << (short)ts->payload.psi.pmt.pcr_pid << std::endl << \
					"Program Info Length: " << ts->payload.psi.pmt.program_info_length << std::endl;

				for (unsigned int p = 0; p < ts->payload.psi.pmt.program_info_length; p++) {
					std::cout << "--> Descriptor information not Implemented yet." << std::endl;
				}

				for (unsigned int p = 0; p < ts->payload.psi.pmt.num_es_info; p++) {
					std::cout << "Elementary Stream Information[" << p << "]: " << std::endl << \
						"\tStream Type: " << (short)ts->payload.psi.pmt.es_info[p].stream_type << std::endl << \
						"\tElementary PID: " << ts->payload.psi.pmt.es_info[p].elementary_PID << std::endl << \
						"\tES Info Length: " << ts->payload.psi.pmt.es_info[p].es_info_length << std::endl;

					for (unsigned int p = 0; p < ts->payload.psi.pmt.es_info[p].es_info_length; p++) {
						std::cout << "--> Descriptor information not Implemented yet." << std::endl;
					}
				}

				std::cout << "CRC: " << ts->payload.psi.pmt.crc << std::endl;
			}
			else {
				// Network Information Table (NIT)
			}
		}

		// Null Packet
		else if (ts->pid == 8191) {
			std::cout << ">>>>> NULL PACKET <<<<<" << std::endl;
		}
	} 
}

#include "tsTransportStream.h"

//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================



/// @brief Reset - reset all TS packet header fields
void xTS_PacketHeader::Reset()
{
	m_SB = 0;
	m_E = 0;
	m_S = 0;
	m_T = 0;
	m_PID = 0;
	m_TSC = 0;
	m_AFC = 0;
	m_CC = 0;
}

/**
  @brief Parse all TS packet header fields
  @param Input is pointer to buffer containing TS packet
  @return Number of parsed bytes (4 on success, -1 on failure)
 */
int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
	const uint32_t* HeadPtr = (const uint32_t*)Input;
	uint32_t Head = xSwapBytes32(*HeadPtr);

	m_SB = (Head & 0b11111111000000000000000000000000) >> 24; // w ten sposob
	if (m_SB != 0x47) return NOT_VALID;

	m_E = (Head & 0b00000000100000000000000000000000) >> 23;
	m_S = (Head & 0b00000000010000000000000000000000) >> 22;
	m_T = (Head & 0b00000000001000000000000000000000) >> 21;
	m_PID = (Head & 0b00000000000111111111111100000000) >> 8;
	m_TSC = (Head & 0b00000000000000000000000011000000) >> 6;
	m_AFC = (Head & 0b00000000000000000000000000110000) >> 4;
	m_CC = (Head & 0b00000000000000000000000000001111);

	return 4;
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
	printf("SB=%d E=%d S=%d T=%d PID=%d TSC=%d AFC=%d CC=%d",
		m_SB, m_E, m_S, m_T, m_PID, m_TSC, m_AFC, m_CC);
}

//=============================================================================================================================================================================
//AF PARSER SECTION
//=============================================================================================================================================================================

/// @brief Reset - reset all TS packet header fields
void xTS_AdaptationField::Reset()
{
	m_AdaptationFieldControl = 0;
	m_AdaptationFieldLength = 0;
	m_Discontinuity = m_RandomAccess = m_ElementaryStreamPriority = m_PCR_flag = m_OPCR_flag = m_SplicingPointFlag = m_TransportPrivateDataFlag = m_AdaptationFieldExtensionFlag = 0;
	PCR_base = OPCR_base = PCR_extension = OPCR_extension = 0;
}
/**
@brief Parse adaptation field
@param PacketBuffer is pointer to buffer containing TS packet
@param AdaptationFieldControl is value of Adaptation Field Control field of
corresponding TS packet header
@return Number of parsed bytes (length of AF or -1 on failure)
*/
int32_t xTS_AdaptationField::Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl)
{
	m_AdaptationFieldControl = AdaptationFieldControl;
	m_AdaptationFieldLength = PacketBuffer[4]; // because header is 32 bits long

	uint8_t AF = PacketBuffer[5];

	m_Discontinuity = (AF & 0b10000000) >> 7;
	m_RandomAccess = (AF & 0b01000000) >> 6;
	m_ElementaryStreamPriority = (AF & 0b00100000) >> 5;
	m_PCR_flag = (AF & 0b00010000) >> 4;
	m_OPCR_flag = (AF & 0b00001000) >> 3;
	m_SplicingPointFlag = (AF & 0b00000100) >> 2;
	m_TransportPrivateDataFlag = (AF & 0b00000010) >> 1;
	m_AdaptationFieldExtensionFlag = (AF & 0b00000001);

	// return 1; // parsed AF
	uint8_t offset = 6;

	if (m_PCR_flag == 1) {
		PCR_base = PCR_base | (PacketBuffer[offset] << 25);
		PCR_base = PCR_base | (PacketBuffer[offset + 1] << 17);
		PCR_base = PCR_base | (PacketBuffer[offset + 2] << 9);
		PCR_base = PCR_base | (PacketBuffer[offset + 3] << 1);
		PCR_base = PCR_base | ((PacketBuffer[offset + 4] >> 7) & 0b1);

		PCR_extension = (PacketBuffer[offset + 4] & 0b1) << 8;
		PCR_extension = PCR_extension | PacketBuffer[offset + 5];
		
		PCR = (PCR_base * xTS::BaseToExtendedClockMultiplier) + PCR_extension;
		
		offset = offset + 6;
	}

	if (m_OPCR_flag == 1) {
		OPCR_base = OPCR_base | (PacketBuffer[offset] << 25);
		OPCR_base = OPCR_base | (PacketBuffer[offset + 1] << 17);
		OPCR_base = OPCR_base | (PacketBuffer[offset + 2] << 9);
		OPCR_base = OPCR_base | (PacketBuffer[offset + 3] << 1);
		OPCR_base = OPCR_base | ((PacketBuffer[offset + 4] >> 7) & 0b1);

		OPCR_extension = (PacketBuffer[offset + 4] & 0b1) << 8;
		OPCR_extension = OPCR_extension | PacketBuffer[offset + 5];

		OPCR = (OPCR_base * xTS::BaseToExtendedClockMultiplier) + OPCR_extension;
		offset = offset + 6;
	}

	if (m_SplicingPointFlag == 1) {
		SpliceCountDown = PacketBuffer[offset];
		offset = offset + 1;
	}

	if (m_TransportPrivateDataFlag) {
		uint8_t TransportPrivateDataLength = PacketBuffer[offset];
		offset = offset + 2 + TransportPrivateDataLength;
	}

	if (m_AdaptationFieldExtensionFlag) {
		uint8_t AdaptationFieldExtensionLength = PacketBuffer[offset];
		offset = offset + 1 + AdaptationFieldExtensionLength;
	}


	//calculating stuffing bytes (-1 cuz offset -4 cuz header)
	StuffingBytes = m_AdaptationFieldLength - (offset -1 - 4);

	return 1;
}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{
	printf("AF: L=%3d DC=%d RA=%d SP=%d PR=%d OR=%d SF=%d TP=%d EX=%d",
		m_AdaptationFieldLength, m_Discontinuity, m_RandomAccess, m_ElementaryStreamPriority, m_PCR_flag, m_OPCR_flag, m_SplicingPointFlag, m_TransportPrivateDataFlag, m_AdaptationFieldExtensionFlag);

	if (m_PCR_flag == 1) {
		double PCR_time = (double)PCR / xTS::ExtendedClockFrequency_Hz;
		printf(" PCR=%08u (Time=%.6lfs)", PCR, PCR_time);
	}

	if (m_OPCR_flag == 1) {
		double OPCR_time = (double)OPCR / xTS::ExtendedClockFrequency_Hz;
		printf(" OPCR=%08X (Time=%.6lfs)", OPCR, OPCR_time);
	}

	if (StuffingBytes > 0) {
		printf(" Stuffing=%u", StuffingBytes);
	}
	else
	{
		printf("Stuffing = 0");
	}

}
//=============================================================================================================================================================================

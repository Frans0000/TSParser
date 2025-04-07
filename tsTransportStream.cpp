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
	m_DC = m_RA = m_SP = m_PR = m_OR = m_SF = m_TP = m_EX = 0;
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

	m_DC = (AF & 0b10000000) >> 7;
	m_RA = (AF & 0b01000000) >> 6;
	m_SP = (AF & 0b00100000) >> 5;
	m_PR = (AF & 0b00010000) >> 4;
	m_OR = (AF & 0b00001000) >> 3;
	m_SF = (AF & 0b00000100) >> 2;
	m_TP = (AF & 0b00000010) >> 1;
	m_EX = (AF & 0b00000001);
	return 1; // parsed AF

}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{
	printf("AF: L=%3d DC=%d RA=%d SP=%d PR=%d OR=%d SF=%d TP=%d EX=%d",
		m_AdaptationFieldLength, m_DC, m_RA, m_SP, m_PR, m_OR, m_SF, m_TP, m_EX);
}
//=============================================================================================================================================================================

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

// 00 reserved for future use
// 01 no adaptation field, only payload
// 10 adaptation field only, no payload
// 11 adaptation field followed by payload

//=============================================================================================================================================================================

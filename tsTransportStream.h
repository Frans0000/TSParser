#pragma once
#include "tsCommon.h"
#include <string>

/*
MPEG-TS packet:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |                             Header                            | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   4 |                  Adaptation field + Payload                   | `
`     |                                                               | `
` 184 |                                                               | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `


MPEG-TS packet header:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |       SB      |E|S|T|           PID           |TSC|AFC|   CC  | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `

Sync byte                    (SB ) :  8 bits
Transport error indicator    (E  ) :  1 bit
Payload unit start indicator (S  ) :  1 bit
Transport priority           (T  ) :  1 bit
Packet Identifier            (PID) : 13 bits
Transport scrambling control (TSC) :  2 bits
Adaptation field control     (AFC) :  2 bits
Continuity counter           (CC ) :  4 bits
*/


//=============================================================================================================================================================================

class xTS
{
public:
  static constexpr uint32_t TS_PacketLength  = 188;
  static constexpr uint32_t TS_HeaderLength  = 4;

  static constexpr uint32_t PES_HeaderLength = 6;

  static constexpr uint32_t BaseClockFrequency_Hz         =    90000; //Hz
  static constexpr uint32_t ExtendedClockFrequency_Hz     = 27000000; //Hz
  static constexpr uint32_t BaseClockFrequency_kHz        =       90; //kHz
  static constexpr uint32_t ExtendedClockFrequency_kHz    =    27000; //kHz
  static constexpr uint32_t BaseToExtendedClockMultiplier =      300;
};

//=============================================================================================================================================================================

class xTS_PacketHeader
{
public:
  enum class ePID : uint16_t
  {
    PAT  = 0x0000,
    CAT  = 0x0001,
    TSDT = 0x0002,
    IPMT = 0x0003,
    NIT  = 0x0010, //DVB specific PID
    SDT  = 0x0011, //DVB specific PID
    NuLL = 0x1FFF,
  };

protected:
    uint8_t  m_SB;
    uint8_t m_E;
    uint8_t m_S;
    uint8_t m_T;
    uint16_t m_PID;
    uint8_t m_TSC;
    uint8_t m_AFC;
    uint8_t m_CC;

public:
  void     Reset();
  int32_t  Parse(const uint8_t* Input);
  void     Print() const;

public:
  uint8_t  getSyncByte() const { return m_SB; } // Sync byte                     (SB ) :  8 bits
  uint8_t  getE() const { return m_E; }         // Transport error indicator    (E  ) :  1 bit
  uint8_t  getS() const { return m_S; }         // Payload unit start indicator (S  ) :  1 bit
  uint8_t  getT() const { return m_T; }         // Transport priority           (T  ) :  1 bit
  uint16_t  getPID() const { return m_PID; }     // Packet Identifier            (PID) : 13 bits
  uint8_t  getTSC() const { return m_TSC; }     // Transport scrambling control (TSC) :  2 bits
  uint8_t  getAFC() const { return m_AFC; }     // Adaptation field control     (AFC) :  2 bits
  uint8_t  getCC() const { return m_CC; }       // Continuity counter           (CC ) :  4 bits

public:
    bool     hasAdaptationField() const { return (m_AFC == 2 || m_AFC == 3); };
    bool     hasPayload() const { return (m_AFC == 1 || m_AFC == 3); };
};

class xTS_AdaptationField
{
protected:
    //setup
    uint8_t m_AdaptationFieldControl;
    //mandatory fields
    uint8_t m_AdaptationFieldLength; //nuber of bytes in the adaptattion field following this byte
    uint8_t m_DC; // indicates that the discontinuity state is true (1)
    uint8_t m_RA; // TS containts some information to aid random access (1)
    uint8_t m_SP; // the payload has a higher priority than other (1)
    uint8_t m_PR; // Program Clock Reference flag
    uint8_t m_OR; // Original Program Clock Reference flag
    uint8_t m_SF; // Splicing point flag
    uint8_t m_TP; // Transport private data flag
    uint8_t m_EX; // Adaptation field extension flag 
    


    //optional fields - PCR skip for now
public:
    void Reset();
    int32_t Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl);
    void Print() const;
public:
    //mandatory fields
    uint8_t getAdaptationFieldLength() const {
        return
            m_AdaptationFieldLength;
    }
    //derived values
    //uint32_t getNumBytes () const { }
};




//=============================================================================================================================================================================

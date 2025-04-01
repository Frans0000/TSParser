#include "tsCommon.h"
#include "tsTransportStream.h"


#include <cstdio>
#include <cstdlib>

//=============================================================================================================================================================================

int main(int argc, char* argv[], char* envp[])
{
    // TODO - open file
    // TODO - check if file if opened

    //if(argc != 2)
    //{
    //  return EXIT_FAILURE;
    //}

    FILE* file = fopen(argv[1], "rb"); //argv[1]

    if (!file)
    {
        fprintf(stderr, "couldnt open file\n");
        return EXIT_FAILURE;
    }

    xTS_PacketHeader    TS_PacketHeader;

    int32_t TS_PacketId = 0;

    const int32_t PacketSize = xTS::TS_PacketLength;

    uint8_t PacketBuffer[PacketSize];


    while (fread(PacketBuffer, 1, PacketSize, file) == PacketSize)
    {
        // TODO - read from file
        TS_PacketHeader.Reset();
        if (TS_PacketHeader.Parse(PacketBuffer) == NOT_VALID) {
            fprintf(stderr, "Invalid packet at ID: %d\n", TS_PacketId);
        }

        printf("%010d ", TS_PacketId);
        TS_PacketHeader.Print();
        printf("\n");

        TS_PacketId++;
        if (TS_PacketId > 15) {
            break;
        }
    }

    // TODO - close file
    fclose(file);

    return EXIT_SUCCESS;
}

//=============================================================================================================================================================================

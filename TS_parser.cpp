#include "tsCommon.h"
#include "tsTransportStream.h"


#include <cstdio>
#include <cstdlib>

//=============================================================================================================================================================================

int main(int argc, char* argv[], char* envp[])
{

    FILE* file = fopen(argv[1], "rb");

    if (!file)
    {
        fprintf(stderr, "couldnt open file\n");
        return EXIT_FAILURE;
    }

    xTS_PacketHeader    TS_PacketHeader;

    int32_t TS_PacketId = 0;

    const int32_t PacketSize = xTS::TS_PacketLength;

    uint8_t PacketBuffer[PacketSize];

    //AF SECTION
    xTS_AdaptationField TS_AdaptationField;

    while (fread(PacketBuffer, 1, PacketSize, file) == PacketSize)
    {
        TS_PacketHeader.Reset();
        if (TS_PacketHeader.Parse(PacketBuffer) == NOT_VALID) {
            fprintf(stderr, "Invalid packet at ID: %d\n", TS_PacketId);
        }



        if (TS_PacketHeader.hasAdaptationField()) {
            TS_AdaptationField.Reset();
            TS_AdaptationField.Parse(PacketBuffer, TS_PacketHeader.getAFC());
            if ((TS_PacketHeader.getPID() == 174)) {
                printf("%010d ", TS_PacketId);
                TS_PacketHeader.Print();

                printf(" ");
                TS_AdaptationField.Print();
                printf("\n");
            }
        }
        TS_PacketId++;

    }

    fclose(file);

    return EXIT_SUCCESS;
}


//=============================================================================================================================================================================

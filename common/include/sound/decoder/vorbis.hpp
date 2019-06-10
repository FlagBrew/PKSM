#ifndef DECODER_VORBIS_H
#define DECODER_VORBIS_H

#include "Decoder.hpp"

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

class VorbisDecoder : public Decoder {
    public:
        VorbisDecoder(const std::string& filename);

        ~VorbisDecoder(void);

        uint32_t pos(void) override;

        uint32_t length(void) override;

        uint32_t decode(void* buffer) override;

        bool stereo(void) override;

        uint32_t sampleRate(void) override;

        uint32_t bufferSize(void) override;

    private:
        uint64_t fillVorbisBuffer(char* bufferOut);

        FILE                *f;
        OggVorbis_File      vorbisFile;
        vorbis_info         *vi;
        const size_t        buffSize = 8 * 4096;
};

int isVorbis(const std::string& in);

#endif

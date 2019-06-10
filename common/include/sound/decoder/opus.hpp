#ifndef DECODER_OPUS_H
#define DECODER_OPUS_H

#include "Decoder.hpp"
#include <opusfile.h>

class OpusDecoder : public Decoder {
    public:
        OpusDecoder(const std::string& filename);

        ~OpusDecoder(void);

        uint32_t pos(void) override;

        uint32_t length(void) override;

        uint32_t decode(void* buffer) override;

        bool stereo(void) override;

        uint32_t sampleRate(void) override;

        uint32_t bufferSize(void) override;

    private:
        uint64_t fillOpusBuffer(int16_t* bufferOut);

        OggOpusFile*      opusFile;
        const size_t      buffSize = 32 * 1024;
};

int isOpus(const std::string& in);

#endif

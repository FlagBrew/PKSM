#ifndef DECODER_WAV_H
#define DECODER_WAV_H

#include "Decoder.hpp"

#include <dr_libs/dr_wav.h>

class WavDecoder : public Decoder {
    public:
        WavDecoder(const std::string& filename);

        ~WavDecoder(void);

        uint32_t pos(void) override;

        uint32_t length(void) override;

        uint32_t decode(void* buffer) override;

        bool stereo(void) override;

        uint32_t sampleRate(void) override;

        uint32_t bufferSize(void) override;

    private:
        const size_t    buffSize    = 16 * 1024;
        drwav*          pWav        = NULL;
        uint32_t        wavprogress;
};

#endif

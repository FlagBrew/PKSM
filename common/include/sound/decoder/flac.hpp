#ifndef DECODER_FLAC_H
#define DECODER_FLAC_H

#include "Decoder.hpp"

#include <dr_libs/dr_flac.h>

class FlacDecoder : public Decoder {
    public:
        FlacDecoder(const std::string& filename);

        ~FlacDecoder(void);

        uint32_t pos(void) override;

        uint32_t length(void) override;

        uint32_t decode(void* buffer) override;

        bool stereo(void) override;

        uint32_t sampleRate(void) override;

        uint32_t bufferSize(void) override;

    private:
        drflac*  pFlac = NULL;
        size_t   buffSize = 16 * 1024;
        uint32_t flacprogress = 0;
};

#endif

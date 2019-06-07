#ifndef DECODER_FLAC_H
#define DECODER_FLAC_H

#include "Decoder.hpp"

class FlacDecoder : public Decoder {
    public:
        FlacDecoder(const char* filename);
        
        ~FlacDecoder(void);
        
        uint32_t pos(void) override;
        
        uint32_t length(void) override;
        
        uint32_t decode(void* buffer) override;

        bool stereo(void) override;
        
        uint32_t sampleRate(void) override;
        
        uint32_t bufferSize(void) override;
};

#endif

#ifndef DECODER_OPUS_H
#define DECODER_OPUS_H

#include "Decoder.hpp"

class OpusDecoder : public Decoder {
    public:
        OpusDecoder(const char* filename);
        
        ~OpusDecoder(void);
        
        uint32_t pos(void) override;
        
        uint32_t length(void) override;
        
        uint32_t decode(void* buffer) override;

        bool stereo(void) override;
        
        uint32_t sampleRate(void) override;
        
        uint32_t bufferSize(void) override;
};

int isOpus(const char* in);

#endif

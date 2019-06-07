#ifndef DECODER_WAV_H
#define DECODER_WAV_H

#include "Decoder.hpp"

class WavDecoder : public Decoder {
    public:
        WavDecoder(const char* filename);
        
        ~WavDecoder(void);
        
        uint32_t pos(void) override;
        
        uint32_t length(void) override;
        
        uint32_t decode(void* buffer) override;

        bool stereo(void) override;
        
        uint32_t sampleRate(void) override;
        
        uint32_t bufferSize(void) override;
};

#endif

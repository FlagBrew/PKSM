#ifndef DECODER_MP3_H
#define DECODER_MP3_H

#include "Decoder.hpp"

class Mp3Decoder : public Decoder {
    public:
        Mp3Decoder(const char* filename);
        
        ~Mp3Decoder(void);
        
        uint32_t pos(void) override;
        
        uint32_t length(void) override;
        
        uint32_t decode(void* buffer) override;

        bool stereo(void) override;
        
        uint32_t sampleRate(void) override;
        
        uint32_t bufferSize(void) override;
};

#endif

#ifndef DECODER_VORBIS_H
#define DECODER_VORBIS_H

#include "Decoder.hpp"

class VorbisDecoder : public Decoder {
    public:
        VorbisDecoder(const char* filename);
        
        ~VorbisDecoder(void);
        
        uint32_t pos(void) override;
        
        uint32_t length(void) override;
        
        uint32_t decode(void* buffer) override;

        bool stereo(void) override;
        
        uint32_t sampleRate(void) override;
        
        uint32_t bufferSize(void) override;
};

int isVorbis(const char* in);

#endif

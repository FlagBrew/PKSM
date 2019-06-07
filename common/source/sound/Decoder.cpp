#include <cstring>
#include <cstdio>

#include "Decoder.hpp"

#include "mp3.hpp"
#include "wav.hpp"
#include "flac.hpp"
#include "vorbis.hpp"
#include "opus.hpp"

std::shared_ptr<Decoder> Decoder::get(const std::string& fileName)
{
    FILE* fp = fopen(fileName.c_str(), "r");
    char magic[4];

    if (!fp)
    {
        fprintf(stderr, "Err: Tried to get magic an audio file that does not exist?!");
        return nullptr;
    }

    fseek(fp, 0, SEEK_SET);
    fread(magic, 1, 4, fp);
    fseek(fp, 0, SEEK_SET);

    /*Wave*/
    if (!strncmp(magic, "RIFF", 4))
    {
        fprintf(stderr, "Decoder: Using wav.");
        auto wavdec = std::shared_ptr<Decoder>(new WavDecoder(fileName.c_str()));
            if (wavdec->good())
                return wavdec;
    }
    /*Flac*/
    else if (!strncmp(magic, "fLaC", 4))
    {
        fprintf(stderr, "Decoder: Using flac.");
        auto flacdec = std::shared_ptr<Decoder>(new FlacDecoder(fileName.c_str()));
            if (flacdec->good())
                return flacdec;
    }
    /*Ogg or Opus*/
    else if (!strncmp(magic, "OggS", 4))
    {
        if (isOpus(fileName.c_str()) == 0)
        {
            fprintf(stderr, "Decoder: Using ogg opus");
            auto opusdec = std::shared_ptr<Decoder>(new OpusDecoder(fileName.c_str()));
            if (opusdec->good())
                return opusdec;
        }
        else if (isVorbis(fileName.c_str()) == 0)
        {
            fprintf(stderr, "Decoder: Using ogg vorbis");
            auto vorbisdec = std::shared_ptr<Decoder>(new VorbisDecoder(fileName.c_str()));
            if (vorbisdec->good())
                return vorbisdec;
        }
        else
            fprintf(stderr, "Decoder: Unknown ogg codec");
    }
    /*Mp3*/
    else if (!strncmp(magic, "ID3", 3))
    {
        fprintf(stderr, "Decoder: Using mpeg3");
        auto mp3dec = std::shared_ptr<Decoder>(new Mp3Decoder(fileName.c_str()));
        if (mp3dec->good())
            return mp3dec;
    }

    fprintf(stderr, "Err: Failed to initalize decoder.");
    return nullptr;
}

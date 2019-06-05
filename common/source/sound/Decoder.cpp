#include <cstring>
#include <cstdio>

#include "Decoder.hpp"

#include "mp3.hpp"
#include "opus.hpp"
#include "vorbis.hpp"

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
        return nullptr;
    }
    /*Flac*/
    else if (!strncmp(magic, "fLaC", 4))
    {
        return nullptr;
    }
    /*Ogg or Opus*/
    else if (!strncmp(magic, "OggS", 4))
    {
        if (!isVorbis(fileName.c_str()))
        {
            auto vorbisdec = std::shared_ptr<Decoder>(new VorbisDecoder(fileName.c_str()));
            return vorbisdec;
        }
        else if (!isOpus(fileName.c_str()))
        {
            auto opusdec = std::shared_ptr<Decoder>(new OpusDecoder(fileName.c_str()));
            return opusdec;
        }
    }
    /*Mp3*/
    else if (!strncmp(magic, "ID3", 3))
    {
        auto mp3dec = std::shared_ptr<Decoder>(new Mp3Decoder(fileName.c_str()));
        return mp3dec;
    }

    return nullptr;
}

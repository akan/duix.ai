#ifndef COMPRESSED_STREAMS_ZSTD_STREAM_H
#define COMPRESSED_STREAMS_ZSTD_STREAM_H

#include <iostream>





class GaesIStream: public std::istream
{
public:
    GaesIStream(std::string filename);

    virtual ~GaesIStream();
};





#endif // COMPRESSED_STREAMS_ZSTD_STREAM_H

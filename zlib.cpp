// Copyright 2007 Timo Bingmann <tb@panthema.net>
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)
//
// Original link http://panthema.net/2007/0328-ZLibString.html

#include <iostream>
#include <string.h>
#include <stdexcept>
#include <iomanip>
#include <sstream>

#include <zlib.h>

class test {
	public:
	std::string compress(const std::string&, int);
	std::string decompress(const std::string&);
};

/** Compress a STL string using zlib with given compression level and return* the binary data. */
std::string test::compress(const std::string& str, int compressionlevel = Z_BEST_COMPRESSION) {
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, compressionlevel) != Z_OK)
        throw(std::runtime_error("deflateInit failed while compressing."));

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();           // set the z_stream's input

    int ret;
    char outbuffer[10240];
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

/** Decompress an STL string using zlib and return the original data. */
std::string test::decompress(const std::string& str) {
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
        throw(std::runtime_error("inflateInit failed while decompressing."));

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();

    int ret;
    char outbuffer[10240];
    std::string outstring;

    // get the decompressed bytes blockwise using repeated calls to inflate
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
            << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

  
test zlib;
  
int main(int argc, char** argv) {
	std::string data = "This is test content just ignore this. hello hello hdd ndddgj hgg iiii hhh dsss";
	
	std::string com = zlib.compress(data, 9);
	std::cerr << "Deflated data: " << data.size() << " -> " << com.size() << " (" << std::setprecision(1) << std::fixed << ( (1.0 - (float)com.size() / (float)data.size()) * 100.0) << "% decrease).\n";
	std::cout << com << "\n";
	
	
	std::string decom = zlib.decompress(com);
	std::cerr << "Inflated data: " << com.size() << " -> " << decom.size() << " (" << std::setprecision(1) << std::fixed << ( ((float)decom.size() / (float)com.size() - 1.0) * 100.0 ) << "% increase).\n";
	std::cout << decom << "\n";
	
	return 0;
}
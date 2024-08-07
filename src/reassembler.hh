#pragma once

#include "byte_stream.hh"
#include <unordered_map>
#include <map>
#include <utility>
class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ) ,First_UnassembledIndex(0),ending_index(0),ending_flag(false),hashmap(){}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;

  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }

private:

  ByteStream output_; // the Reassembler writes to this ByteStream

  //当前internalbuffer里面暂存的字节数
  //uint64_t BufferPending;
  //当前的乱序部分的首字节index
  uint64_t First_UnassembledIndex;
  //结束字节的索引号，初始化为0；
  uint64_t ending_index;
  //为了标识结束索引号是否有效
  bool ending_flag;
  //暂存的map
  std::map<uint64_t,std::string> hashmap;
  //现在是一个byte对应一个index，所以我们需要对每一个byte进行存储
};

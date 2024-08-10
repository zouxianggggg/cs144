#pragma once

#include "byte_stream.hh"
#include <unordered_map>
#include <set>
#include <vector>
#include <utility>
#include <string>
struct interval
{
  /* data */
  uint64_t start_idx;
  uint64_t end_idx;
  std::string data;

  //定义两个interval之间的大小关系
  bool operator<(const interval& other) const {
    if(start_idx == other.start_idx)
    {
      return end_idx < other.end_idx;
    }
    return start_idx < other.start_idx;
  };
};


class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ) {}

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
  //把每一段的data的开始index和结束index都直接显示出来，然后就是区间合并的问题
  std::set<interval> buffers {};
  uint64_t first_unassembled_index = 0;                                         
  uint64_t eof_index = UINT64_MAX;  
  ByteStream output_; // the Reassembler writes to this ByteStream

  
};

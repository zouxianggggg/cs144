#include "wrapping_integers.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return Wrap32 { static_cast<uint32_t>( ( zero_point.raw_value_ + n ) % ( 1UL << 32 ) ) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // 就是找到所有zero_point对应的absolute seq中最接近checkpoint的结果

  // 先计算checkpoint对应的倍数n
  uint64_t cpn = ( checkpoint & ( 0xFFFFFFFF00000000 ) ) >> 32;
  uint64_t currentRaw; // 这是this.raw_value对应的第一个absolute seq，然后再乘上对应的倍数进行比较即可
  if ( this->raw_value_ > zero_point.raw_value_ ) {
    currentRaw = static_cast<uint64_t>( this->raw_value_ ) - static_cast<uint64_t>( zero_point.raw_value_ );
  } else {
    currentRaw
      = static_cast<uint64_t>( this->raw_value_ ) + ( 1UL << 32 ) - static_cast<uint64_t>( zero_point.raw_value_ );
  }
  uint64_t prob1 = ( cpn << 32 ) + currentRaw;
  uint64_t prob2 = ( ( cpn + 1 ) << 32 ) + currentRaw;
  uint64_t prob3 = ( ( cpn - 1 ) << 32 ) + currentRaw;
  uint64_t m1 = prob1 > checkpoint ? prob1 - checkpoint : checkpoint - prob1;
  uint64_t m2 = prob2 > checkpoint ? prob2 - checkpoint : checkpoint - prob2;
  uint64_t m3 = prob3 > checkpoint ? prob3 - checkpoint : checkpoint - prob3;
  uint64_t mm = min( m1, min( m2, m3 ) );
  if ( mm == m1 ) {
    return prob1;
  } else if ( mm == m2 ) {
    return prob2;
  } else {
    return prob3;
  }
  return {};
}

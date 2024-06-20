#include "wrapping_integers.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32 { uint32_t((n+zero_point.raw_value_)%( uint64_t(1)<<32)) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
   uint64_t base = uint64_t(1) << 32;
   uint64_t divider = checkpoint >> 32;	   
   uint64_t remainder = checkpoint % base;
   uint64_t offset;

   if(zero_point.raw_value_ > raw_value_ ){
    offset = raw_value_ +  UINT32_MAX - zero_point.raw_value_ + 1;
   } else {
    offset = raw_value_ - zero_point.raw_value_; 
   }
   
   if(offset > remainder){
    if(offset - remainder > 1UL<<31){
     if(divider > 0){
      return ((divider-1)<<32) + offset;
     } else {
      return (divider<<32) + offset;
     }	     
    } else {
     return (divider<<32) + offset;
    }	    
   } else {
    if(remainder - offset > 1UL<<31){
     return ((divider+1)<<32) + offset;
    } else {
     return (divider<<32) + offset;
    }	    
   }	   
   /*
   uint64_t candi_l = base * divider + offset, candi_r = base * (divider + 1) + offset; 

   if(candi_l >= checkpoint ){
    return candi_l;
   } else {
    return (checkpoint - candi_l) < (candi_r - checkpoint) ? candi_l : candi_r;
   }
   */   
}

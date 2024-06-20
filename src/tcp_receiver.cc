#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  
  if(message.SYN) { 
   syn = message.seqno;
   is_start = true;
  }	  
  
  if(message.RST){
   reassembler_.set_error();
  }	  

  if(is_start) { 
   bool is_last_bytes = message.FIN;

   uint64_t index = message.seqno.unwrap(syn, reassembler_.get_next()); 
   if(message.SYN) index += 1; 
   reassembler_.insert(index-1, message.payload, is_last_bytes);
  }
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here
  uint16_t window;
  if(writer().available_capacity() > UINT16_MAX) {
   window = UINT16_MAX;
  } else {
   window = uint16_t(writer().available_capacity());
  }	  

  if(is_start){
   if(!writer().is_closed()){	  
    return {Wrap32::wrap(reassembler_.get_next()+1, syn), window, reader().has_error()};
   } else {
    return {Wrap32::wrap(reassembler_.get_next()+2, syn), window, reader().has_error()};  	    
   }	   
  } else {
   return {nullopt , window, reader().has_error()};
  }	  
}  

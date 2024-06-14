#include "byte_stream.hh"
#include<iostream>
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), write_ptr(0), read_ptr(0), closed_(false), queue({}), buffer_queue({}){
}

bool Writer::is_closed() const
{
  return closed_;
}

void Writer::push( string data )
{
   if(available_capacity() == 0 || data.size() == 0 ) return;
   if(available_capacity() < data.size()){
   	data = data.substr(0, available_capacity());
   
   }
   const auto n = data.size(); 
   write_ptr += n;   
   queue.push_back(std::move(data));
   buffer_queue.emplace_back(queue.back().c_str(), n);

   
   /*
   string_view data_view(data);
   uint64_t data_size = data.size();
   if(available_capacity() < data_size){
    data_view = data_view.substr(0, available_capacity());
    data_size = available_capacity();
   }	   
   if(write_ptr%capacity_ > (write_ptr+data_size-1)%capacity_){
    buffer.replace(write_ptr%capacity_, capacity_ - write_ptr%capacity_, data_view.substr(0, capacity_-write_ptr%capacity_)); 
    buffer.replace(0, data_size - (capacity_ - write_ptr%capacity_), data_view.substr(capacity_-write_ptr%capacity_, data_size - (capacity_-write_ptr%capacity_)));
   } else { 
    buffer.replace(write_ptr%capacity_, data_size, data_view);
   }	   
   
   write_ptr += data_size;
   */   
}

void Writer::close()
{
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - (write_ptr - read_ptr);
}

uint64_t Writer::bytes_pushed() const
{
  return write_ptr;
}

bool Reader::is_finished() const
{
  return (write_ptr==read_ptr)&&closed_;
}

uint64_t Reader::bytes_popped() const
{
  return read_ptr;
}

string_view Reader::peek() const
{
  if(bytes_buffered() > 0){
   return buffer_queue.front();
  //std::string_view ret(buffer.data()+read_ptr%capacity_, 1);
  
  } else {
   std::string_view ret("");
   return ret;
  }	  
}

void Reader::pop( uint64_t len )
{
   if(len > bytes_buffered()){
   	len = bytes_buffered();
   }	   
   read_ptr = read_ptr + len;
   while(len > 0){
    if(len >= buffer_queue.front().size()){
     len -= buffer_queue.front().size();	    
     buffer_queue.pop_front();
     queue.pop_front();
    } else {
     buffer_queue.front().remove_prefix(len);	    
     len = 0;
    }	    
   }	   
}

uint64_t Reader::bytes_buffered() const
{
  return write_ptr - read_ptr;
}

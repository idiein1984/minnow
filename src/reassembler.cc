#include "reassembler.hh"
#include<utility>
#include<algorithm>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  auto& write = writer(); 
  if(data.size() == 0){
   if(first_index == next_ && is_last_substring) {
    last_byte_ = next_;
    write.close();
   }	 
   return;  
  }	  
  if(is_last_substring && first_index + data.size() < last_byte_){
   last_byte_ = first_index + data.size();
  }	  

  if(first_index + data.size()-1 < next_ ){
   return;
  }	  
  
  // do sth when over capacity_
  if(first_index > write.available_capacity() + next_ - 1){
   return;
  }	  
  if(first_index + data.size() - 1 > write.available_capacity() + next_ - 1){
   data = data.substr(0, next_+write.available_capacity()-first_index);
  }	  


  uint64_t start, end;
  if(first_index <= next_){
   data = data.substr(next_ - first_index, data.size()-(next_-first_index));
   
   start = max(first_index, next_);
   end = start + data.size() - 1;
   add_storage(start, end , data);
   while(!storage_.empty() && write.available_capacity() > 0){
    auto firstItem = *(storage_.begin());
    if(firstItem.first.first == next_) {
     
     storage_.erase(storage_.begin());
     if(write.available_capacity() >= firstItem.second.size()){
      write.push(firstItem.second);
      next_ += firstItem.second.size();
     } else {
      next_ += write.available_capacity(); 
      storage_[make_pair(firstItem.first.first+write.available_capacity(), firstItem.first.second)] = firstItem.second.substr(write.available_capacity(), firstItem.second.size()-write.available_capacity());      
      write.push(firstItem.second.substr(0, write.available_capacity()));
     }	     
     
     if(next_ == last_byte_){
      write.close();
     }	     
    } else {
     break;	    
    }	    
   }	   
  } else {	 
   start = first_index;
   end = start + data.size()-1;
   add_storage(start, end, data); 
  }
}

uint64_t Reassembler::bytes_pending() const
{
  uint64_t ret = 0;
  for(const auto& item: storage_){
   ret += item.second.size();
  }	  
  return ret;
}

void Reassembler::add_storage(uint64_t start, uint64_t end, string data){
 
 for(auto it = storage_.begin(); it != storage_.end();){
  if(it->first.first <= start && end <= it->first.second){
   return;
  }	  
  
  if(start < it->first.first && it->first.first <= end && end < it->first.second ){
   end = it->first.first-1;
   data = data.substr(0, end - start + 1);
  }	  
  
  if(it->first.first < start && start <= it->first.second && it->first.second < end){
   start = it->first.second+1;
   data = data.substr(data.size()-(end-start+1) ,end-start + 1);
  }	  

  if(start <= it->first.first && it->first.second <= end){
   storage_.erase(it++);
  } else {
   ++it;
  }	  
 }	
 
 storage_[make_pair(start, end)] = data; 
}	

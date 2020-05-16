#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  
  long int min,s,h;
   min = (seconds % 3600) / 60;
   s = (seconds % 3600) % 60;
   h = seconds / 3600;
  
  char array[9];
  sprintf(array, "%.2ld:%.2ld:%.2ld", h, min, s);
  string req_string(array);
  return req_string;
  
  
 }
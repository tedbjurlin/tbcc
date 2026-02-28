#include "strwindow.h"

StrWindow create_str_window(char *src, int size) {
  if (!src) {
    return (StrWindow){
      .ptr = src,
      .size = -1
    };
  }
  if (size == 0) {
    return (StrWindow){
      .ptr = src,
      .size = 0
    };
  }
  
  for (int i = 0; i < size; ++i) {
    if (*(src + i) == '\0') {
      return (StrWindow){
        .ptr = src,
        .size = i
      };
    }
  }
  
  return (StrWindow){
    .ptr = src,
    .size = size
  };
}
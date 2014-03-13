///
/// This file is part of The Pivot framework.
/// Written by Gabriel Dos Reis <gdr@cs.tamu.edu>
/// 

#include <algorithm>

#include "utility.H"

/// Support for measuring how much memory IPR datastructures take
#ifdef IPR_TRACK_MEMORY_SIZE
namespace ipr 
{
    namespace stats
    {
        extern int ipr_mem_size;
    }
}
#endif ///< IPR_TRACK_MEMORY_SIZE


char
ipr::util::string::operator[](int i) const
{
   if (i < 0 || i >= length)
      throw std::domain_error("invalid index for util::string::operator[]");
   return data[i];
}

ipr::util::string::arena::arena()
      : mem(static_cast<pool*>(operator new(poolsz))),
        next_header(mem->storage)
{
   mem->previous = 0;
}

ipr::util::string::arena::~arena()
{
   while (mem != 0) {
      pool* cur = mem;
      mem = mem->previous;
      operator delete (cur);
   }
}

/// Allocate storage sufficient to hold an immutable string of length "n".

ipr::util::string*
ipr::util::string::arena::allocate(int n)
{

   /// Support for measuring how much memory IPR datastructures take
   #ifdef IPR_TRACK_MEMORY_SIZE
   stats::ipr_mem_size += n;
   #endif ///< IPR_TRACK_MEMORY_SIZE


   const int m = (n - string::padding_count + headersz - 1) / headersz + 1;
   string* header;

   /// If we have enough space left, juts grab it.
   if (m <= remaining_header_count()) {
      header = next_header;
      next_header += m;
   }
   /// If we need to allocate storage more than what can possibly fit
   /// in a fresh pool object, just allocate that string on its own
   else if (n > bufsz) {
      pool* new_pool = static_cast<pool*>
         (operator new(poolsz + (n - bufsz)));
      header = new_pool->storage;

      new_pool->previous = mem->previous;
      mem->previous = new_pool;
   }
   /// Not enough space left.  Take the bet that, there does not
   /// remain sufficient room in the buffer.  This is likely so if
   /// the buffer is allocated sufficiently large to start with.
   else {
      pool* new_pool = static_cast<pool*>(operator new(poolsz));
      new_pool->previous = mem;
      mem = new_pool;

      header = mem->storage;
      next_header = header + m;
   }

   return header;
}

const ipr::util::string*
ipr::util::string::arena::make_string(const char* s, int n)
{
   string* header = allocate(n);

   header->length = n;

   /// Put cast to avoid gettinch assert with safe STL in MSVC
   std::copy(s, s + n, (char*)header->data);

   return header;
}

/*
 * =====================================================================================
 *
 *       Filename:  test_move.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013/8/6 14:59:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huys (hys), huys03@gmail.com
 *        Company:  hu
 *
 * =====================================================================================
 */
#include <memory>
#include <string>
#include <cassert>

template <typename T>
class Buffer
{
   std::string          _name;
   size_t               _size;
   std::unique_ptr<T[]> _buffer;

public:
   // default constructor
   Buffer():
      _size(16),
      _buffer(new T[16])
   {}

   // constructor
   Buffer(const std::string& name, size_t size):
      _name(name),
      _size(size),
      _buffer(new T[size])
   {}

   // copy constructor
   Buffer(const Buffer& copy):
      _name(copy._name),
      _size(copy._size),
      _buffer(new T[copy._size])
   {
      T* source = copy._buffer.get();
      T* dest = _buffer.get();
      std::copy(source, source + copy._size, dest);
   }

   // copy assignment operator
   Buffer& operator=(const Buffer& copy)
   {
      if(this != &copy)
      {
         _name = copy._name;

         if(_size != copy._size)
         {
            _buffer = nullptr;
            _size = copy._size;
            _buffer = _size > 0 ? new T[_size] : nullptr;
         }

         T* source = copy._buffer.get();
         T* dest = _buffer.get();
         std::copy(source, source + copy._size, dest);
      }

      return *this;
   }

   // move constructor
   Buffer(Buffer&& temp):
      _name(std::move(temp._name)),
      _size(temp._size),
      _buffer(std::move(temp._buffer))
   {
      temp._buffer = nullptr;
      temp._size = 0;
   }

   // move assignment operator
   Buffer& operator=(Buffer&& temp)
   {
      assert(this != &temp); // assert if this is not a temporary

      _buffer = nullptr;
      _size = temp._size;
      _buffer = std::move(temp._buffer);

      _name = std::move(temp._name);

      temp._buffer = nullptr;
      temp._size = 0;

      return *this;
   }
};

template <typename T>
Buffer<T> getBuffer(const std::string& name)
{
   Buffer<T> b(name, 128);
   return b;
}

int main()
{
   Buffer<int> b1;
   Buffer<int> b2("buf2", 64);
   Buffer<int> b3 = b2;
   Buffer<int> b4 = getBuffer<int>("buf4");
   b1 = getBuffer<int>("buf5");
   return 0;
}

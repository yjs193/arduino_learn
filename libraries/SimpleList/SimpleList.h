// SimpleList.h

#ifndef _SIMPLELIST_h
#define _SIMPLELIST_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef NULL
#define NULL 0
#endif

template<typename T>
class SimpleList
{
public:
    typedef T** iterator;

    SimpleList()
    {
        _internalArray = NULL;
        _endPosition = 0;
        _allocBlocks = 0;
        _preAllocBlocks = 0;
    }

    ~SimpleList()
    {
        delete[] _internalArray;
        _internalArray = NULL;
        _endPosition = 0;
        _allocBlocks = 0;
        _preAllocBlocks = 0;
    }

    SimpleList(const SimpleList& from)
    {
        _endPosition = from._endPosition;
        _allocBlocks = from._allocBlocks;
        _preAllocBlocks = from._preAllocBlocks;

        _internalArray = new T*[_allocBlocks];

        for (int i = 0; i < _endPosition; ++i)
            _internalArray[i] = from._internalArray[i];
    }

    SimpleList& operator=(const SimpleList& from)
    {
        if (this != &from)
        {
            _endPosition = from._endPosition;
            _allocBlocks = from._allocBlocks;
            _preAllocBlocks = from._preAllocBlocks;

            delete[] _internalArray;
            _internalArray = NULL;

            if (_allocBlocks)
            {
                _internalArray = new T*[_allocBlocks];

                for (int i = 0; i < _endPosition; ++i)
                    _internalArray[i] = from._internalArray[i];
            }
        }

        return *this;
    }

    void push_back(T item)
    {
        if (_endPosition == _allocBlocks)
            AllocOneBlock(false);

        _internalArray[_endPosition] = new T(item);
        ++_endPosition;
    }

    void push_front(T item)
    {
        if (_endPosition == _allocBlocks)
            AllocOneBlock(true);
        else
        {
            for (int i = _endPosition; i > 0; --i)
                _internalArray[i] = _internalArray[i - 1];
        }

        _internalArray[0] = new T(item);
        ++_endPosition;
    }

    void insert(int index, T item)
    {
        if(index > _endPosition){
            return;
        }
        if (_endPosition == _allocBlocks){
            AllocOneBlock(false);
        }

        for (int i = _endPosition; i >= index; --i){
          _internalArray[i] = _internalArray[i - 1];
        }

        _internalArray[index] = new T(item);
        ++_endPosition;
    }
    
    void replace(int index, T item)
    {
      if((_allocBlocks == 0) || (index > (_endPosition-1)) || (index < 0)){
          return ;
      }
      delete _internalArray[index];
      _internalArray[index] = new T(item);
    }
    
    T operator[](int index)
    {
        return *_internalArray[index];
    }
    
    void remove(int index){
      if((_allocBlocks == 0) || (index > (_endPosition-1)) || (index < 0)){
          return ;
      }
      erase(_internalArray+index);
    }
    
    bool contains(T item)
    {
        for(iterator i=begin();i!= end(); i++){
            if(**i == item) 
                return true;
        }
        return false;
    }
    
    void pop_back()
    {
        if (_endPosition == 0)
            return;
        delete _internalArray[--_endPosition];
    }

    void pop_front()
    {
        if (_endPosition == 0)
            return;

        --_endPosition;

        if (_allocBlocks > _preAllocBlocks){
            DeAllocOneBlock(true);
        }else{
            for (int i = 0; i < _endPosition; ++i){
                _internalArray[i] = _internalArray[i + 1];
            }
        }
    }

    iterator erase(iterator position)
    {
        int offSet = int(position - _internalArray);
        if (offSet == _endPosition - 1) // Last item.
        {
            pop_back();
            return end();
        }

        --_endPosition;

        if (_allocBlocks > _preAllocBlocks)
        {
            --_allocBlocks;
            T** newArray = new T*[_allocBlocks];

            for (int i = 0; i < _endPosition; ++i)
            {
                if(i == offSet){
                    newArray[i] = _internalArray[i + 1];
                    delete _internalArray[i];
                }else if (i > offSet){
                    newArray[i] = _internalArray[i + 1];
                }else{
                    newArray[i] = _internalArray[i];
                }
            }

            delete[] _internalArray;
            _internalArray = newArray;
        }
        else
        {
            for (int i = offSet; i < _endPosition; ++i)
                _internalArray[i] = _internalArray[i + 1];
        }

        return _internalArray + offSet;
    }

    void reserve(int size)
    {
        if (size == 0 || size < _allocBlocks)
            return;

        _allocBlocks = size;
        _preAllocBlocks = size;

        T** newArray = new T*[_allocBlocks];

        for (int i = 0; i < _endPosition; ++i){
            newArray[i] = _internalArray[i];
        }

        delete[] _internalArray;
        _internalArray = newArray;
    }

    void clear()
    {
        if (_allocBlocks > _preAllocBlocks)
        {
            _allocBlocks = _preAllocBlocks;

            T** newArray = NULL;

            if (_allocBlocks > 0)
                newArray = new T*[_allocBlocks];
            for(int i=0; i<_endPosition; i++){
                delete _internalArray[i];
            }
            delete[] _internalArray;
            _internalArray = newArray;
        }

        _endPosition = 0;
    }

    void shrink_to_fit()
    {
        _preAllocBlocks = _endPosition;
        _allocBlocks = _endPosition;

        T** newArray = NULL;

        if (_allocBlocks > 0)
            newArray = new T*[_allocBlocks];

        for (int i = 0; i < _endPosition; ++i){
            newArray[i] = _internalArray[i];
            //delete _internalArray[i];
        }

        delete[] _internalArray;
        _internalArray = newArray;
    }

    inline iterator begin() { return _internalArray; }
    inline iterator end() { return _internalArray + _endPosition; }

    inline bool empty() { return (_endPosition == 0); }
    inline unsigned int size() { return _endPosition; }
    inline unsigned int capacity() { return _allocBlocks; }

private:

    void AllocOneBlock(bool shiftItems)
    {
        ++_allocBlocks;
        T** newArray = new T*[_allocBlocks];
        for (int i = 0; i < _endPosition; ++i){
            newArray[shiftItems ? (i + 1) : i] = _internalArray[i];
        }
        delete[] _internalArray;
        _internalArray = newArray;
    }

    void DeAllocOneBlock(bool shiftItems)
    {
        --_allocBlocks;
        delete _internalArray[0];
        if (_allocBlocks == 0)
        {
            delete[] _internalArray;
            _internalArray = NULL;
            return;
        }

        T** newArray = new T*[_allocBlocks];

        for (int i = 0; i < _endPosition; ++i){
            newArray[i] = _internalArray[shiftItems ? (i + 1) : i];
        }

        delete[] _internalArray;
        _internalArray = newArray;
    }

public:

    String listToString(SimpleList<String> li){
        String list = "{";
        for(int i = 0; i < li.size(); i++)
        {
            list = list + i + ": \"" + li[i] + "\"";
            if(i < li.size() -1) list += ", ";
        }
        list += "}";
        return list;
    }
    uint8_t indexOf(String str){
        SimpleList<String> p = *this;
        for(int i = 0; i < p.size(); i++){
            if(p[i] == str) return i+1;
        }
        return 0;
    }

    T** _internalArray;
    int _endPosition;
    int _allocBlocks;
    int _preAllocBlocks;
};

#endif
#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <vector>
#include <iostream>
#include <mutex>
#include <cassert>

#include "SDL2/SDL.h"


namespace util
{
    class CircularBuffer
    {
    public:  

        //circularBuffer(circularBuffer &b);
        CircularBuffer();
        CircularBuffer(int sr, int s);
        CircularBuffer(int length);
        void setLen(int length);
        inline size_t size() const {return len;};
        void clear();

        void fill(Uint8 *src, size_t size);
        void feed(std::vector<float> &dst, size_t size);
        void peep(std::vector<float>& _data, int& pr, int& pw, int& ps);

        size_t getLen() {return len;};
        size_t getResLen();
        size_t getReadPosition();
        size_t getWritePosition();

        float& operator[](size_t ind);
        std::mutex m_mutex;

    private:
        std::vector<float> data;
        size_t len;
        size_t wPos;
        size_t rPos;
        size_t resLen;
        bool reachmax;
        bool allowWrap;
        int samplerate;
        int seconds;

    };
}

#endif // CIRCULARBUFFER_H
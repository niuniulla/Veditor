#include "CircularBuffer.hpp"


namespace util
{ 
    // circularBuffer::circularBuffer(circularBuffer &b): m_mutex()
    // {
    //     data = b.data;
    //     len = b.len;
    //     wPos = b.wPos;
    //     rPos =b.rPos;
    //     resLen = b.resLen;
    //     reachmax = b.reachmax;
    //     allowWrap = b.allowWrap;
    //     samplerate = b.samplerate;
    //     seconds = b.seconds;
    // }

    /* circularbuffer*/
    CircularBuffer::CircularBuffer()
    {  
        samplerate = 0;
        seconds = 0;
        data.clear();
        len = 0; // maximal length of hte buffer = sample rate X seconds
        wPos = 0; // write position
        rPos = 0; // read position
        resLen = 0; // the rest of the length that can be filled
        reachmax = false;
        allowWrap = true; 
    }

    /* circularbuffer*/
    CircularBuffer::CircularBuffer(int length)
    {
        // samplerate = sr;
        // seconds = s;
        // length = sr * s
        len = length; // maximal length of hte buffer = sample rate X seconds
        data.resize(len);
        std::fill(data.begin(), data.end(), 0);
        wPos = 0; // write position
        rPos = 0; // read position
        resLen = 0; // the rest of the length that can be filled
        reachmax = false;
        allowWrap = true; 
    }

    void CircularBuffer::setLen(int length)
    {
        len = length; // maximal length of hte buffer = sample rate X seconds
        data.resize(len);
    }

    // function to reset the buffer
    void CircularBuffer::clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::fill(data.begin(), data.end(), 0);
        rPos = 0;
        wPos = 0;
        resLen = 0;
        reachmax = false;
    }

    // overloaded operator to read the data at a position
    float& CircularBuffer::operator[](size_t ind)
    {
        return data[ind];
    }

    // function to fill the buffer
    // both wirte and read positions and reslen are updated
    void CircularBuffer::fill(Uint8 *src, size_t newLen)
    {
        //size_t newFloatLen = byteLen / sizeof(_Float32);

        std::lock_guard<std::mutex> lock(m_mutex);
        // if longuer than the buffer, we take only the last samples
        // TODO
        if (newLen > len)
        {
            reachmax = true;
        }

        int res = (wPos + newLen) - len;

        // if the buffer capacity is reached, wrap the surplus
        if (res > 0)
        {
            reachmax = true;
            
            if (allowWrap)
            {
                size_t firstLen = len - wPos;

                SDL_memcpy(&data[wPos], src, firstLen * sizeof(_Float32));
                SDL_memcpy(&data[0], src + firstLen * sizeof(_Float32), res*sizeof(_Float32));

                wPos = (wPos + newLen) % len;

                // if writing pos is behind the reading pos,
                if (res <= static_cast<int>(rPos))
                { 
                    resLen = len - rPos + res;
                }
                else
                {
                    std::cout << "WARNING - data loss in circular buffer." << std::endl;
                    // if the input pos is ahead of the reading pos, 
                    // there is a data loss due to the overwriting of the old data
                    resLen = len;
                    // we have to advance the reading pos accordingly, otherwise, the old and new data will be mixed up
                    rPos = wPos;
                }
            }   
        }
        else
        {
            SDL_memcpy(&data[wPos], src, newLen*sizeof(float));
            
            wPos = (wPos + newLen) % len;
            resLen = wPos - rPos;
        }
    }

    // function to retrieve the data from buffer
    // both wirte and read positions and resLen are updated
    void CircularBuffer::feed(std::vector<float> &dst, size_t outLen)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // if the required len shorter than available, return
        if (outLen <= resLen)
        { 
            if (rPos < wPos)
            {
                if ((rPos + outLen) <= len)
                {
                    SDL_memcpy(&dst[0], &data[rPos], sizeof(float) * outLen);
                }
                else
                {
                    int resl = len - (rPos + outLen);
                    SDL_memcpy(&dst[0], &data[rPos], sizeof(float) * (len-rPos));
                    SDL_memcpy(&dst[len-rPos], &data[0], sizeof(float) * resl);
                }
    
                //std::cout << "DEBUG - buffers size: " << resLen << " " << wPos << " " << rPos << std::endl;
            }
            else
            {
                int l = len - rPos - 1;
                SDL_memcpy(&dst[0], &data[rPos], sizeof(float)*l);
                SDL_memcpy(&dst[l], &data[0], sizeof(float)*(outLen-l));  
            }
            rPos = (rPos + outLen) % len;
    
            resLen -= outLen;
            //std::cout << "DEBUG - len: " << wPos << " " << rPos << " " << outLen << " " << resLen << std::endl;
        }
    }

    // function to read the data from buffer
    // no changes to the buffer
    void CircularBuffer::peep(std::vector<float>& _data, int& pr, int& pw, int& ps)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        SDL_memcpy(_data.data(), data.data(), sizeof(float) * len);
        pr = rPos;
        pw = wPos;
        ps = resLen;
    }

    size_t CircularBuffer::getResLen()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return resLen;
    }

    size_t CircularBuffer::getReadPosition()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return rPos;
    }

    size_t CircularBuffer::getWritePosition()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return wPos;
    }

}
#include "AudioUtil.hpp"

#define PI 3.1415926
#define SIGNMASK(x) (Uint32)(0u - ((Uint32)(x) >> 31))
union float_bits {
    Uint32 u32;
    float f32;
};

namespace util
{
    void FileManager::open_wav(const std::string filename, std::vector<float> &outbuffer)
    {
        SDL_AudioSpec wavSpec;

        Uint32 wavLength;
        Uint8 *wavBuffer;

        SDL_LoadWAV(filename.c_str(), &wavSpec, &wavBuffer, &wavLength);

        Uint32 outLen = wavLength / sizeof(float);
        outbuffer.resize(outLen);
        SDL_memcpy(&outbuffer[0], wavBuffer, wavLength);
    }

    // https://en.wikipedia.org/wiki/Low-pass_filter#Frequency_response
    void _IIF_simple_low_pass(std::vector<float> buf, const float cutf, const float sampler)
    {
        float rc = 1.0f / (2.0f * PI * cutf);
        float dt = 1.0f / sampler;

        float alpha = dt / (dt + rc);

        for (int i=1; i<buf.size(); i++)
        {
            float tmp = alpha * (buf[i] - buf[i-1]);
            buf[i] += tmp;
        }
    }

    // https://en.wikipedia.org/wiki/High-pass_filter
    void _IIF_simple_high_pass(std::vector<float> &buf, const float cutf, const float sampler)
    {
        float rc = 1.0f / (2.0f * PI * cutf);
        float dt = 1.0f / sampler;

        float alpha = rc / (dt + rc);

        float xold = buf[0];

        for (int i=1; i<buf.size(); i++)
        { 
            float tmp = alpha * (buf[i-1] + buf[i] - xold);
            xold = buf[i];
            buf[i] += tmp;
        }
    }

    bool vad_naive(std::vector<float> &buf, float threshold, float e0, bool doFilter, float cutf_low, float cutf_high, float sampler)
    {
        // filter
        if (doFilter)
        {
            _IIF_simple_low_pass(buf, cutf_low, sampler);
            _IIF_simple_high_pass(buf, cutf_high, sampler);
        }

        float e_total = 0.0;
        for (int i=0; i<buf.size(); i++)
        {
            e_total += fabs(buf[i] * buf[i]);
        }
        e_total /= (buf.size() * buf.size());

        float epsilon = 1e-7;
        float e_log = 10.0 * log10((e_total + epsilon) / e0);
        //std::cout << "db: " << e_log << " ";

        if (e_log > threshold)
            return true;
            
        return false;
    }

    void SDL_Convert_F32_to_S32_Scalar(Sint32 *dst, const float *src, int num_samples)
    {
        int i;

        for (i = 0; i < num_samples; ++i) {
            float_bits x = { .f32 = src[i] };

            /* 1) Shift the float range from [-1.0, 1.0] to [-2147483648.0, 2147483648.0]
             * 2) Set values outside the [-2147483648.0, 2147483647.0] range to -2147483648.0
             * 3) Convert the float to an integer, and fixup values outside the valid range */
            Uint32 y = x.u32 + 0x0F800000u;
            Uint32 z = y - 0xCF000000u;
            z &= SIGNMASK(y ^ z);
            x.u32 = y - z;

            dst[i] = (Sint32)((Uint32)(Sint32)x.f32 ^ SIGNMASK(z));
        }
    }
}


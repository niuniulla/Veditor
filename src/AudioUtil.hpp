#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include "SDL2/SDL.h"

namespace util
{
    struct FileManager
    {
        void open_wav(const std::string filename, std::vector<float> &outbuffer);
    };

    void _IIF_simple_low_pass(std::vector<float> buf, const float cutf, const float sampler);

    void _IIF_simple_high_pass(std::vector<float> &buf, const float cutf, const float sampler);

    bool vad_naive(std::vector<float> &buf, float threshold, float e0=1e1, bool doFilter=false, float cutf_low=0.0f, float cutf_high=0.0f, float sampler=0);
    
    void SDL_Convert_F32_to_S32_Scalar(Sint32 *dst, const float *src, int num_samples);

} // namespace qheditor


#endif // FILEUTIL_H
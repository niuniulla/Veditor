#ifndef WHISPERUTIL_H
#define WHISPERUTIL_H

#include <iostream>
#include <vector>
#include <thread>

#include "../lib/whispercpp/whisper.h"

#include "./SDLAudioUtil.hpp"
#include "./AudioUtil.hpp"


namespace util
{
    
    struct whisper_params
    {
        int32_t n_threads = std::min(4, (int32_t)std::thread::hardware_concurrency());
        int32_t n_processors = 1;
        int32_t offset_t_ms = 0;
        int32_t offset_n = 0;
        int32_t duration_ms = 0;
        int32_t progress_step = 5;
        int32_t max_context = -1;
        int32_t max_len = 0;
        int32_t best_of = whisper_full_default_params(WHISPER_SAMPLING_GREEDY).greedy.best_of;
        int32_t beam_size = whisper_full_default_params(WHISPER_SAMPLING_BEAM_SEARCH).beam_search.beam_size;
        int32_t audio_ctx = 0;

        float word_thold = 0.01f;
        float entropy_thold = 2.40f;
        float logprob_thold = -1.00f;

        bool speed_up = false;
        bool debug_mode = false;
        bool translate = false;
        bool detect_language = false;
        bool diarize = false;
        bool tinydiarize = false;
        bool split_on_word = false;
        bool no_fallback = false;
        bool output_txt = false;
        bool output_vtt = false;
        bool output_srt = false;
        bool output_wts = false;
        bool output_csv = false;
        bool output_jsn = false;
        bool output_jsn_full = false;
        bool output_lrc = false;
        bool no_prints = false;
        bool print_special = false;
        bool print_colors = false;
        bool print_progress = false;
        bool no_timestamps = false;
        bool log_score = false;
        bool use_gpu = true;

        std::string language = "en";
        std::string prompt;
        std::string font_path = "/System/Library/Fonts/Supplemental/Courier New Bold.ttf";
        std::string model = "/resources/models/ggml-tiny.en.bin";

        // [TDRZ] speaker turn string
        std::string tdrz_speaker_turn = " [SPEAKER_TURN]"; // TODO: set from command line

        std::string openvino_encode_device = "CPU";

        std::vector<std::string> fname_inp = {};
        std::vector<std::string> fname_out = {};
    };

    struct whisper_callback_user_data
    {
        const whisper_params *params;
        std::string *text;
        int progress_prev = 0;
    };

    struct StreamOptions
    {
        bool useVad = true;
        bool useFilter = true;
        bool changeLen = false;
        float vadThreshold = -50.0f;
        float filterLow = 20.0f;
        float filterHigh = 200.0f;
        float samplerate = 16000.0f;
        float e0 = 1.0f;
        int lengthSecond = 3;
    };

    class WhisperCTX
    {
        public:

            WhisperCTX() = default;
            ~WhisperCTX() {whisper_free(m_ctx);};

            bool init(whisper_params params, int sr);

            void transcribe(std::vector<float> pcmf32, std::string& out);
            void transcribe(std::vector<float> pcmf32, void *user_data);
            void stream(CircularBuffer *buffer, StreamOptions &ops, std::string& out);

            static void whisper_out_segment_callback(struct whisper_context *ctx, struct whisper_state * /*state*/, int n_new, void *user_data);

            void showDebugInfo();

        private:
            whisper_params m_params; // user parames
            whisper_full_params m_wParams; // whisper inference parameters
            struct whisper_context_params m_ctxParams; // ctx parameters
            struct whisper_context* m_ctx;
            size_t m_streamBufferSize;
            int m_sampleRate;

    };
}

#endif // WHISPERUTIL_H
#include "WhisperUtil.hpp"

namespace util
{ 
    bool WhisperCTX::init(whisper_params params, int sr)
    {
        m_params = params;
        m_ctxParams = whisper_context_default_params();
        m_ctxParams.use_gpu = m_params.use_gpu;

        // initialize whisper context
        m_ctx = whisper_init_from_file_with_params(m_params.model.c_str(), m_ctxParams);

        if (!m_ctx)
        {
            std::cerr << "ERR - Failed to initialize whisper context using file:" << m_params.model << std::endl;
            return false;
        }
        
        // only support english
        if (m_params.language != "en")
        {
            std::cerr << "ERR - Language not supported." << std::endl;
            return false;
        }

        // setup inference parameters
        m_wParams                       = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
        m_wParams.strategy              = m_params.beam_size > 1 ? WHISPER_SAMPLING_BEAM_SEARCH : WHISPER_SAMPLING_GREEDY;

        m_wParams.print_realtime        = false;
        m_wParams.print_progress        = m_params.print_progress;
        m_wParams.print_timestamps      = !m_wParams.no_timestamps;
        m_wParams.print_special         = m_params.print_special;
        m_wParams.translate             = m_params.translate;
        m_wParams.language              = m_params.language.c_str();
        m_wParams.detect_language       = m_params.detect_language;
        m_wParams.n_threads             = m_params.n_threads;
        m_wParams.n_max_text_ctx        = m_params.max_context >= 0 ? m_params.max_context : m_wParams.n_max_text_ctx;
        m_wParams.offset_ms             = m_params.offset_t_ms;
        m_wParams.duration_ms           = m_params.duration_ms;

        m_wParams.token_timestamps      = m_params.output_wts || m_params.output_jsn_full || m_wParams.max_len > 0;
        m_wParams.thold_pt              = m_params.word_thold;
        m_wParams.max_len               = m_params.output_wts && m_wParams.max_len == 0 ? 60 : m_wParams.max_len;
        m_wParams.split_on_word         = m_params.split_on_word;
        m_wParams.audio_ctx             = m_params.audio_ctx;

        m_wParams.speed_up              = m_params.speed_up;
        m_wParams.debug_mode            = m_params.debug_mode;

        m_wParams.tdrz_enable           = m_params.tinydiarize; // [TDRZ]

        m_wParams.initial_prompt        = m_params.prompt.c_str();

        m_wParams.greedy.best_of        = m_params.best_of;
        m_wParams.beam_search.beam_size = m_params.beam_size;

        m_wParams.temperature_inc       = m_params.no_fallback ? 0.0f : m_wParams.temperature_inc;
        m_wParams.entropy_thold         = m_params.entropy_thold;
        m_wParams.logprob_thold         = m_params.logprob_thold;

        m_wParams.no_timestamps         = m_params.no_timestamps;

        m_sampleRate = sr;

        std::cerr << "INFO - streaming buffer size: " << 3*sr << std::endl;
        m_streamBufferSize = 3 * sr; // we set 3s as seg length for streaming
        
        return true;
    }

    void WhisperCTX::transcribe(std::vector<float> pcmf32, std::string& out)
    {
        if (whisper_full_parallel(m_ctx, m_wParams, pcmf32.data(), pcmf32.size(), m_params.n_processors) != 0)
        {
            std::cout << "ERR - Failed to process audio." << std::endl;
            return;
        }
        int numSeg = whisper_full_n_segments(m_ctx);
        for (int i=0; i<numSeg; i++)
        {
            const char *segtext = whisper_full_get_segment_text(m_ctx, i);
            out.append(segtext);
        }
    }

    void WhisperCTX::transcribe(std::vector<float> pcmf32, void *user_data)
    {
        m_wParams.print_realtime = false;
        m_wParams.print_progress = false;
        // whisper_print_user_data user_data = {&m_params, &pcmf32s, 0};

        // this callback is called on each new segment

        m_wParams.new_segment_callback = whisper_out_segment_callback;
        m_wParams.new_segment_callback_user_data = &user_data;

        if (whisper_full_parallel(m_ctx, m_wParams, pcmf32.data(), pcmf32.size(), m_params.n_processors) != 0)
        {
            std::cout << "ERR - Failed to process audio." << std::endl;
            return;
        }
    }

    void WhisperCTX::whisper_out_segment_callback(struct whisper_context *ctx, struct whisper_state * /*state*/, int n_new, void *user_data)
    {
        auto &params = *((whisper_callback_user_data *)user_data)->params;
        auto &text = *((whisper_callback_user_data *)user_data)->text;

        const int n_seg = whisper_full_n_segments(ctx);

        const int print_s = n_seg - n_new;

        if (print_s == 0)
        {
            text += '\n';
        }

        for (int i=0; i<n_seg; i++)
        {
            const char *seg_str = whisper_full_get_segment_text(ctx, i);
            std::cout << "INFO - transcribed text - seg " << i << " :" << seg_str << std::endl;
            text += seg_str;
        }
        text =+ '\n';
    }

    void WhisperCTX::stream(CircularBuffer *buffer, StreamOptions &ops, std::string& out)
    {
        out = "";
        std::vector<float> pcmf32(m_sampleRate*ops.lengthSecond, 0);
        bool bReadyForInference = false;
        
        if (buffer->getResLen() >= m_streamBufferSize)
        {
            buffer->feed(pcmf32, m_streamBufferSize);
            bReadyForInference = true; 
        }
        
        //naive vad
        bool vad = true;
        if (ops.useVad)
        {
            vad = vad_naive(pcmf32, ops.vadThreshold, ops.e0, ops.useFilter, ops.filterLow, ops.filterHigh, ops.samplerate);
            std::cout << "vad: " << vad << std::endl;
        }
        
        if (bReadyForInference && vad)
        {

            if (whisper_full(m_ctx, m_wParams, pcmf32.data(), pcmf32.size()) != 0)
            //if (whisper_full_parallel(m_ctx, m_wParams, pcmf32.data(), pcmf32.size(), 3) != 0)
            {
                std::cout << "ERR - failed to process audio." << std::endl;
                return;
            }

            // output text
            int n_seg = whisper_full_n_segments(m_ctx);
            if (n_seg)
            {
                for (int i=0; i<n_seg; i++)
                {
                    const char *text = whisper_full_get_segment_text(m_ctx, i);
                    out.append(text);
                }
            }

            //std::cout << "DEBUG - outut text: " << out << std::endl;
        }
    }
}
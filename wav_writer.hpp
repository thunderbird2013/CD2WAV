// wav_writer.hpp
#pragma once
#include <string>
#include <vector>
#include <cstdint>

class WAVWriter {
public:
    static bool write(const std::string& filename,
                      const std::vector<uint8_t>& pcm_data,
                      uint32_t sample_rate,
                      uint16_t channels,
                      uint16_t bits_per_sample);
};

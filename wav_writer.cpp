// wav_writer.cpp
#include "wav_writer.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

static void write_little_endian(std::ofstream& out, uint32_t value, size_t size) {
    for (size_t i = 0; i < size; ++i)
        out.put(static_cast<char>((value >> (8 * i)) & 0xFF));
}

bool WAVWriter::write(const std::string& filename_utf8,
                      const std::vector<uint8_t>& data,
                      uint32_t sample_rate,
                      uint16_t channels,
                      uint16_t bits_per_sample)
{
    // UTF-8 → wchar*
    int wlen = MultiByteToWideChar(CP_UTF8, 0, filename_utf8.c_str(), -1, nullptr, 0);
    std::wstring wfilename(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, filename_utf8.c_str(), -1, &wfilename[0], wlen);

    HANDLE hFile = CreateFileW(
        wfilename.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    DWORD written;
    auto write = [&](const void* data, DWORD len) {
        return WriteFile(hFile, data, len, &written, NULL);
    };

    uint32_t byte_rate = sample_rate * channels * bits_per_sample / 8;
    uint16_t block_align = channels * bits_per_sample / 8;
    uint32_t data_size = static_cast<uint32_t>(data.size());
    uint32_t chunk_size = 36 + data_size;

    write("RIFF", 4); 
    write(&chunk_size, 4); 
    write("WAVEfmt ", 8);
    uint32_t subchunk1_size = 16; uint16_t format = 1;
    write(&subchunk1_size, 4); 
    write(&format, 2); 
    write(&channels, 2);
    write(&sample_rate, 4); 
    write(&byte_rate, 4); 
    write(&block_align, 2);
    write(&bits_per_sample, 2);
    write("data", 4); 
    write(&data_size, 4);
    write(data.data(), data_size);
    CloseHandle(hFile);
    return true;
}


// cdda_reader.hpp
#pragma once
#include <string>
#include <vector>
#include <cdio/cdio.h>
#include <cdio/paranoia/cdda.h>
#include <cdio/paranoia/paranoia.h>

class CDDAReader {
public:
    CDDAReader();
    ~CDDAReader();

    bool open(const std::string& path);          // CD oder ISO öffnen
    bool load_toc();                             // TOC laden
    int get_track_count() const;                 // Track-Anzahl
    bool read_track(int track_number, std::vector<uint8_t>& pcm_out); // PCM lesen
    int get_track_offset(int track_number) const;
    int get_total_disc_length_frames() const;

private:
    CdIo_t* cdio = nullptr;
    cdrom_drive_t* drive = nullptr;
    cdrom_paranoia_t* paranoia = nullptr;
    int first_track = 0;
    int last_track = 0;
};

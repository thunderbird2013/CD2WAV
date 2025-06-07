// cdda_reader.cpp
#include "cdda_reader.hpp"
#include <cdio/cdio.h>
#include <cdio/cd_types.h>
#include <cdio/logging.h>
#include <cdio/cdtext.h>
#include <cdio/audio.h>
#include <iostream>
#include <fstream>

CDDAReader::CDDAReader() : cdio(nullptr) {}

bool CDDAReader::open(const std::string& path) {
    cdio = cdio_open(path.c_str(), DRIVER_BINCUE);
    if (!cdio) {
        std::cerr << "[CDDAReader] Konnte CD-Quelle nicht öffnen." << std::endl;
        return false;
    }
    std::cout << "[CDDAReader] CD-Quelle geöffnet." << std::endl;
    return true;
}

bool CDDAReader::load_toc() {
    if (!cdio) return false;

    first_track = cdio_get_first_track_num(cdio);
    last_track = cdio_get_last_track_num(cdio);

    return first_track <= last_track;
}

int CDDAReader::get_track_count() const {
    return last_track - first_track + 1;
}

int CDDAReader::get_track_offset(int track_num) const {
    if (!cdio || track_num < first_track || track_num > last_track)
        return -1;
    return cdio_get_track_lsn(cdio, track_num);
}

int CDDAReader::get_total_disc_length_frames() const {
    if (!cdio) return -1;
    return cdio_get_disc_last_lsn(cdio);
}

bool CDDAReader::read_track(int track_number, std::vector<uint8_t>& pcm_out) {
    if (!cdio) return false;

    lsn_t start = cdio_get_track_lsn(cdio, track_number);
    lsn_t end;

    if (track_number < last_track) {
        end = cdio_get_track_lsn(cdio, track_number + 1) - 1;
    } else {
        end = cdio_get_disc_last_lsn(cdio);
    }

    if (start == CDIO_INVALID_LSN || end == CDIO_INVALID_LSN || end <= start) {
        std::cerr << "[CDDAReader] Ungültiger LSN-Bereich für Track " << track_number << std::endl;
        return false;
    }

    long total_sectors = end - start + 1;
    const size_t bytes_per_sector = CDIO_CD_FRAMESIZE_RAW;
    pcm_out.resize(total_sectors * bytes_per_sector);

    for (long i = 0; i < total_sectors; ++i) {
        unsigned char* buffer = &pcm_out[i * bytes_per_sector];
        int read = cdio_read_audio_sector(cdio, buffer, start + i);
        if (read != 0) {
            std::cerr << "[CDDAReader] Fehler beim Lesen von Sektor " << (start + i) << std::endl;
            return false;
        }
    }

    return true;
}

CDDAReader::~CDDAReader() {
    if (cdio) {
        cdio_destroy(cdio);
    }
}

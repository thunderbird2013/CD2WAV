// cdda_reader.cpp
#include "cdda_reader.hpp"
#include <cdio/cdio.h>
#include <cdio/logging.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

CDDAReader::CDDAReader() : cdio(nullptr) {}

bool CDDAReader::open(const std::string& path) {
    const struct {
        const char* name;
        driver_id_t driver;
        } 
        driver_options[] =          {
        { "BINCUE",  DRIVER_BINCUE  },
        { "NRG",     DRIVER_NRG     },     // funktioniert nur, wenn libcdio NRG unterstützt
        { "DEVICE",  DRIVER_DEVICE  },
        { "WIN32",   DRIVER_WIN32   },
    };

    for (const auto& entry : driver_options) {
        cdio = cdio_open(path.empty() ? nullptr : path.c_str(), entry.driver);
        if (cdio) {
            std::cout << "[CDDAReader] Geöffnet mit: " << entry.name << "\n";
            return true;
        }
    }

    std::cerr << "[CDDAReader] Öffnen fehlgeschlagen: " << path << "\n";
    return false;
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
    if (!cdio) {
        std::cerr << "[CDDAReader] cdio nicht initialisiert.\n";
        return false;
    }

    lsn_t start = cdio_get_track_lsn(cdio, track_number);
    lsn_t end;

    // Berechne End-LSN abhängig vom Track
    if (track_number < last_track) {
        end = cdio_get_track_lsn(cdio, track_number + 1) - 1;
    } else {
        end = cdio_get_disc_last_lsn(cdio) - 1;  // Letzter gültiger Sektor (inklusiv)
    }

    // Sicherheitskappe auf gültige Disc-Grenze
    lsn_t disc_max = cdio_get_disc_last_lsn(cdio) - 1;
    if (end > disc_max) end = disc_max;

    if (start == CDIO_INVALID_LSN || end == CDIO_INVALID_LSN || end < start) {
        std::cerr << "[CDDAReader] Ungültiger LSN-Bereich für Track " << track_number << ": " << start << " bis " << end << "\n";
        return false;
    }

    const long total_sectors = end - start + 1;
    const size_t bytes_per_sector = CDIO_CD_FRAMESIZE_RAW;
    const size_t total_bytes = static_cast<size_t>(total_sectors) * bytes_per_sector;

    // Schutz vor zu großer Allokation
    if (total_sectors <= 0 || total_bytes == 0 || total_bytes > 100 * 1024 * 1024) {
        std::cerr << "[CDDAReader] Ungültige oder zu große Puffergröße für Track " << track_number << "\n";
        return false;
    }

    try {
        pcm_out.resize(total_bytes);
    } catch (...) {
        std::cerr << "[CDDAReader] Speicherreservierung fehlgeschlagen für Track " << track_number << "\n";
        return false;
    }

    std::cout << "[CDDAReader] Lese Track " << track_number
              << ": Sektoren " << start << " bis " << end
              << " (" << total_sectors << " Sektoren)\n";

    for (long i = 0; i < total_sectors; ++i) {
        const size_t offset = static_cast<size_t>(i) * bytes_per_sector;

        if (offset + bytes_per_sector > pcm_out.size()) {
            std::cerr << "[CDDAReader] Puffergrenze überschritten bei Sektorindex " << i << "\n";
            break;
        }

        unsigned char* buffer = &pcm_out[offset];
        bool success = false;

        for (int attempt = 0; attempt < 5; ++attempt) {
            if (cdio_read_audio_sector(cdio, buffer, start + i) == 0) {
                success = true;
                break;
            }
        }

        if (!success) {
            std::cerr << "[CDDAReader] Fehler beim Lesen von Sektor " << (start + i) << " – ersetzt mit Stille.\n";
            std::memset(buffer, 0, bytes_per_sector);
        }
    }

    return true;
}


CDDAReader::~CDDAReader() {
    if (cdio) {
        cdio_destroy(cdio);
    }
}

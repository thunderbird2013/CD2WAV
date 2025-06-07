#include <cdio/cdio.h>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Bitte .cue-Datei angeben\n";
        return 1;
    }

    CdIo_t* cdio = cdio_open(argv[1], DRIVER_BINCUE);
    if (!cdio) {
        std::cerr << "cdio_open fehlgeschlagen!\n";
        return 1;
    }

    int first = cdio_get_first_track_num(cdio);
    int last  = cdio_get_last_track_num(cdio);
    std::cout << "Tracks: " << first << " bis " << last << std::endl;

    for (int t = first; t <= last; ++t) {
        lsn_t lsn = cdio_get_track_lsn(cdio, t);
        std::cout << "Track " << t << ": Start-LSN " << lsn << std::endl;
    }

    cdio_destroy(cdio);
    return 0;
}

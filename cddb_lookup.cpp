// cddb_lookup.cpp
#include "cddb_lookup.hpp"
#include <cddb/cddb.h>
#include <iostream>
#include <winsock2.h>

bool CDDBLookup::fetch_metadata(const std::vector<int>& track_offsets, int disc_length_frames, DiscInfo& info) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

    if (track_offsets.empty() || disc_length_frames <= 0) {
        std::cerr << "Ungültige TOC-Daten.\n";
        return false;
    }

    cddb_conn_t* conn = cddb_new();
    if (!conn) return false;

    cddb_set_server_name(conn, "gnudb.gnudb.org");
    cddb_set_http_path_query(conn, "/~cddb/cddb.cgi");
    cddb_set_server_port(conn, 80);
    cddb_set_charset(conn, "UTF-8");

    cddb_disc_t* disc = cddb_disc_new();
    cddb_disc_set_length(disc, disc_length_frames / 75);

    for (size_t i = 0; i < track_offsets.size(); ++i) {
        cddb_track_t* track = cddb_track_new();
        cddb_track_set_frame_offset(track, track_offsets[i]);
        cddb_disc_add_track(disc, track);
    }

    cddb_disc_calc_discid(disc);

    if (!cddb_query(conn, disc)) {
        std::cerr << "CDDB-Abfrage fehlgeschlagen: " << cddb_error_str(cddb_errno(conn)) << std::endl;
        cddb_disc_destroy(disc);
        cddb_destroy(conn);
        return false;
    }

    if (!cddb_read(conn, disc)) {
        std::cerr << "CDDB-Lesen fehlgeschlagen: " << cddb_error_str(cddb_errno(conn)) << std::endl;
        cddb_disc_destroy(disc);
        cddb_destroy(conn);
        return false;
    }

    info.title = cddb_disc_get_title(disc);
    info.artist = cddb_disc_get_artist(disc);
    info.genre = cddb_disc_get_genre(disc);
    info.year = std::to_string(cddb_disc_get_year(disc));

    int num_tracks = cddb_disc_get_track_count(disc);
    for (int i = 0; i < num_tracks; ++i) {
        cddb_track_t* track = cddb_disc_get_track(disc, i);
        TrackInfo tinfo;
        tinfo.title = cddb_track_get_title(track);
        tinfo.offset_frames = cddb_track_get_frame_offset(track);
        info.tracks.push_back(tinfo);
    }

    std::cerr << "DiscID: " << std::hex << cddb_disc_get_discid(disc) << std::endl;
    
    cddb_disc_destroy(disc);
    cddb_destroy(conn);
    return true;
}

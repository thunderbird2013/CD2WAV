// main.cpp
#include "cdda_reader.hpp"
#include "wav_writer.hpp"
#include "cddb_lookup.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <windows.h>
#include <bits/basic_string.h>

int main(int argc, char* argv[]) {
    
    // Setze Icon
    HWND hwnd = GetConsoleWindow();
    HICON hIcon = (HICON)LoadImage(NULL, "myapp.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    if (hIcon) {
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }

    SetConsoleOutputCP(CP_UTF8); // UTF-8 aktivieren
    std::cout << "[CHECK] main läuft" << std::endl;
    std::string input_path;
    if (argc > 1) {
        input_path = argv[1];
    } else {
        std::cout << "Pfad zu CD oder ISO angeben: ";
        std::getline(std::cin, input_path);
    }

    CDDAReader reader;
    if (!reader.open(input_path)) {
        std::cerr << "Fehler beim Oeffnen von " << input_path << std::endl;
        return 1;
    }

    if (!reader.load_toc()) {
        std::cerr << "Konnte TOC nicht laden." << std::endl;
        return 1;
    }

    int num_tracks = reader.get_track_count();
    std::cout << "Tracks gefunden: " << num_tracks << std::endl;

    std::vector<int> offsets;
    for (int i = 0; i < num_tracks; ++i) {
        int offset = reader.get_track_offset(i + 1);
        std::cout << "Track " << (i + 1) << " Start-LSN: " << offset << std::endl;
        offsets.push_back(offset);
    }
    int total_frames = reader.get_total_disc_length_frames();
    std::cout << "Gesamtlänge in Frames: " << total_frames << std::endl;

    DiscInfo info;
    bool has_metadata = CDDBLookup::fetch_metadata(offsets, total_frames, info);
    if (has_metadata) {
        std::cout << "Album:  " << info.artist << " - " << info.title << std::endl;
        std::cout << "Genre:  " << info.genre << " | Jahr: " << info.year << std::endl;
    }

    for (int i = 0; i < num_tracks; ++i) {
        std::vector<uint8_t> pcm_data;
        std::cout << "Lese Track " << (i + 1) << "..." << std::endl;

        bool ok = reader.read_track(i + 1, pcm_data);  // 👈 zuerst deklarieren
        std::cout << "[MAIN] Rückgabe read_track: " << ok << std::endl;

        if (!ok) {
            std::cerr << "Fehler beim Lesen von Track " << i + 1 << std::endl;
            continue;
        }

        if (!reader.read_track(i + 1, pcm_data)) {
            
            std::cerr << "Fehler beim Lesen von Track " << i + 1 << std::endl;
            continue;
        }
        std::cout << "PCM-Daten: " << pcm_data.size() << " Bytes" << std::endl;

        std::string filename;
        if (has_metadata && i < (int)info.tracks.size() && !info.tracks[i].title.empty()) {
            filename = "track" + std::to_string(i + 1) + "_" + info.tracks[i].title + ".wav";
        } else {
            filename = "track" + std::to_string(i + 1) + ".wav";
        }

        std::cout << "Speichere: " << filename << std::endl;
        if (!WAVWriter::write(filename, pcm_data, 44100, 2, 16)) {
            std::cerr << "Fehler beim Speichern von " << filename << std::endl;
        } else {
            std::cout << "Gespeichert: " << filename << std::endl;
        }
    }

    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    std::cout << "Arbeitsverzeichnis: " << cwd << std::endl;
    return 0;
}

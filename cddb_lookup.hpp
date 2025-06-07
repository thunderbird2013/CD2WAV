// cddb_lookup.hpp
#pragma once
#include <string>
#include <vector>

struct TrackInfo {
    std::string title;
    int offset_frames;
};

struct DiscInfo {
    std::string title;
    std::string artist;
    std::string genre;
    std::string year;
    std::vector<TrackInfo> tracks;
};

class CDDBLookup {
public:
    static bool fetch_metadata(const std::vector<int>& track_offsets, int disc_length_frames, DiscInfo& info);
};

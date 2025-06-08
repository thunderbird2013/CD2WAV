# 🎵 CDDA-ISO Extractor für Windows (MSYS2)

Dieses Tool extrahiert Audio-Tracks aus einer CD-Audio-ISO-Datei (z. B. BIN/CUE oder NRG), benennt sie optional nach CDDB-Metadaten und speichert sie als `.wav`.

## 🔧 Funktionen

- 📀 Liest CD-ISOs oder echte CD-Laufwerke mit **libcdio**
- 🧠 Holt Metadaten (Album, Titel, Jahr, Tracks) via **CDDB (libcddb)**
- 🔊 Speichert WAV-Dateien mit UTF-8-kompatiblen Dateinamen
- 🚫 Crash-Schutz durch Windows SEH-Handler
- 💾 WAV-Export mit 44100 Hz, 2 Kanäle, 16 Bit

## 🗂️ Projektstruktur

```
main.cpp           → Steuert alles (UI, CDDA, CDDB, WAV)
cdda_reader.cpp    → Liest Audio-Tracks über libcdio
cddb_lookup.cpp    → CDDB-Abfrage über libcddb
wav_writer.cpp     → Speichert WAV-Dateien mit WinAPI
```

## 🛠️ Kompilierung (unter Windows + MSYS2/MinGW64)

### Voraussetzungen

- MSYS2 mit MinGW64
- Pakete:
  ```bash
  pacman -S mingw-w64-x86_64-toolchain \
             mingw-w64-x86_64-libcdio \
             mingw-w64-x86_64-libcddb
  ```

### Kompilierung

```bash
g++ -std=c++17 main.cpp cdda_reader.cpp cddb_lookup.cpp wav_writer.cpp -o cdda_extract.exe \
    -lcdio -lcddb -lole32 -luuid -static -mwindows
```

Optional: Icon-Datei `myapp.ico` im Verzeichnis ablegen für Fenster-Icon.

## ✅ Verwendung

```bash
cdda_extract.exe "CD_IMAGE.BIN"
```

Falls kein Argument übergeben wird, fragt das Programm interaktiv nach einem Pfad.

Die Ausgabe erfolgt in einem Unterordner (z. B. `CD_IMAGE`), mit Tracks als WAV-Dateien:

```
track1_Title1.wav
track2_Title2.wav
...
```

## 📚 Libraries

- [libcdio](https://www.gnu.org/software/libcdio/)
- [libcddb](https://libcddb.sourceforge.net/)
- WinAPI (für WAV-Export & Fensterfunktionen)

## 📦 Lizenz

Dieses Projekt ist frei verwendbar zu Lern- und Forschungszwecken. Kein kommerzieller Support.

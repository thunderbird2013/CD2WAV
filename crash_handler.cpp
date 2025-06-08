#include "crash_handler.hpp"
#include <fstream>
#include <ctime>
#include <DbgHelp.h>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cstdlib>

namespace fs = std::filesystem;

typedef BOOL(WINAPI* MiniDumpWriteDump_t)(
    HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
    const PMINIDUMP_EXCEPTION_INFORMATION,
    const PMINIDUMP_USER_STREAM_INFORMATION,
    const PMINIDUMP_CALLBACK_INFORMATION);

std::string current_timestamp_string() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(t, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

void write_crash_dump(EXCEPTION_POINTERS* pException, const std::string& path) {
    HMODULE dbgHelp = LoadLibraryA("Dbghelp.dll");
    if (!dbgHelp) return;

    auto MiniDumpWriteDump = (MiniDumpWriteDump_t)GetProcAddress(dbgHelp, "MiniDumpWriteDump");
    if (!MiniDumpWriteDump) return;

    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;

    MINIDUMP_EXCEPTION_INFORMATION mdei;
    mdei.ThreadId = GetCurrentThreadId();
    mdei.ExceptionPointers = pException;
    mdei.ClientPointers = FALSE;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &mdei, nullptr, nullptr);
    CloseHandle(hFile);
}

LONG WINAPI global_exception_handler(EXCEPTION_POINTERS* pException) {

    // Zeitstempel zuerst holen!
    std::string timestamp = current_timestamp_string();

    // Absoluter Pfad zur .exe bestimmen
    char exe_path[MAX_PATH];
    GetModuleFileNameA(nullptr, exe_path, MAX_PATH);
    std::filesystem::path base_path = std::filesystem::path(exe_path).parent_path();
    std::filesystem::path crash_dir = base_path / "crash";

    // Ordner sicher erstellen
    std::error_code ec;
    fs::create_directory(crash_dir, ec);

    // Volle Pfade bauen
    std::filesystem::path log_file_path  = crash_dir / ("log_"  + timestamp + ".log");
    std::filesystem::path dump_file_path = crash_dir / ("dump_" + timestamp + ".dmp");

    std::string log_file  = log_file_path.string();
    std::string dump_file = dump_file_path.string();


    DWORD code = pException->ExceptionRecord->ExceptionCode;
    PVOID addr = pException->ExceptionRecord->ExceptionAddress;

    std::string cause;
    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:
        cause = "Speicherzugriffsfehler (Access Violation)";
        break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        cause = "Array-Grenzüberschreitung";
        break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        cause = "Datenfehlanpassung (Datentyp nicht korrekt ausgerichtet)";
        break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        cause = "Division durch Null (float)";
        break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        cause = "Division durch Null (int)";
        break;
    case EXCEPTION_FLT_OVERFLOW:
        cause = "Gleitkommaüberlauf";
        break;
    case EXCEPTION_FLT_UNDERFLOW:
        cause = "Gleitkommaunterlauf";
        break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        cause = "Ungültige CPU-Instruktion";
        break;
    case EXCEPTION_IN_PAGE_ERROR:
        cause = "Zugriffsfehler bei Paging";
        break;
    case EXCEPTION_INT_OVERFLOW:
        cause = "Ganzzahlüberlauf (int)";
        break;
    case EXCEPTION_INVALID_DISPOSITION:
        cause = "Ungültige Ausnahmebehandlung";
        break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        cause = "Nicht fortsetzbare Ausnahme";
        break;
    case EXCEPTION_PRIV_INSTRUCTION:
        cause = "Privilegierte CPU-Instruktion im User-Modus";
        break;
    case EXCEPTION_STACK_OVERFLOW:
        cause = "Stacküberlauf";
        break;
    case EXCEPTION_GUARD_PAGE:
        cause = "Schreibversuch auf GUARD-Seite";
        break;
    case CONTROL_C_EXIT:
        cause = "Strg+C oder Break-Signal";
        break;
    default:
        cause = "Unbekannte Ausnahme";
        break;
    }

   MessageBoxA(nullptr, log_file.c_str(), "Log-Pfad", MB_OK);
   std::ofstream log(log_file);
    if (log) {
        log << "[CRASH_HANDLER] Zeit: " << timestamp << "\n";
        log << "Exception Code: 0x" << std::hex << code << " (" << cause << ")\n";
        log << "Exception Addr: " << addr << "\n";
        log << "Dump File: " << dump_file << "\n";
        log << "Erkannt in: crash_handler.cpp → global_exception_handler()\n";
    } else {
    // 💥 Fallback, wenn std::ofstream fehlschlägt
    HANDLE h = CreateFileA(log_file.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
            std::ostringstream fallback;
            fallback << "[CRASH_HANDLER - Fallback] Zeit: " << timestamp << "\r\n";
            fallback << "Exception Code: 0x" << std::hex << code << " (" << cause << ")\r\n";
            fallback << "Exception Addr: " << addr << "\r\n";
            fallback << "Dump File: " << dump_file << "\r\n";
            fallback << "Erkannt in: crash_handler.cpp → global_exception_handler()\r\n";

            std::string text = fallback.str();
            DWORD written = 0;
            WriteFile(h, text.c_str(), static_cast<DWORD>(text.size()), &written, nullptr);
            CloseHandle(h);
            } else {
            // Wenn auch das fehlschlägt, zeig den Fehler direkt an
            MessageBoxA(nullptr, ("[CRASH_HANDLER] Konnte Log-Datei NICHT schreiben:\n" + log_file).c_str(), "Schwerer Fehler", MB_ICONERROR);
        }
    }

    std::cerr << "[CRASH_HANDLER] Ausnahme erkannt: " << cause << "\n";
    std::cerr << "[CRASH_HANDLER] Dump gespeichert: " << dump_file << "\n";

    write_crash_dump(pException, dump_file);
    std::cerr << "[CRASH_HANDLER] Programm wird jetzt beendet\n";

    std::ostringstream msg;
    msg << "Crash wurde erkannt!\n\n"
    << "Typ: " << cause << "\n"
    << "Code: 0x" << std::hex << code << "\n"
    << "Adresse: " << addr << "\n\n"
    << "Dump-Datei:\n" << dump_file << "\n"
    << "Log-Datei:\n" << log_file;

    MessageBoxA(nullptr, msg.str().c_str(), "Crash erkannt", MB_ICONERROR | MB_OK);

    std::exit(EXIT_FAILURE);
}

void install_crash_handler() {
    SetUnhandledExceptionFilter(global_exception_handler);
}
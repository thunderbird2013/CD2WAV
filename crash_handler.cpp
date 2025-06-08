#include <windows.h>
#include "crash_handler.hpp"
#include <fstream>
#include <ctime>
#include <DbgHelp.h>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <unordered_map>

namespace fs = std::filesystem;
std::string func_name = "<unbekannt>";
std::string file_name = "<unbekannt>";
DWORD line_num = 0;


using MiniDumpWriteDump_t = BOOL(WINAPI*)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
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

LONG WINAPI global_exception_handler(EXCEPTION_POINTERS* exception_info) {
    const std::unordered_map<DWORD, std::string> exception_strings = {
        { EXCEPTION_ACCESS_VIOLATION,           "Speicherzugriffsfehler (Access Violation)" },
        { EXCEPTION_ARRAY_BOUNDS_EXCEEDED,      "Array-Grenzüberschreitung" },
        { EXCEPTION_DATATYPE_MISALIGNMENT,      "Datenfehlanpassung" },
        { EXCEPTION_FLT_DIVIDE_BY_ZERO,         "Division durch Null (float)" },
        { EXCEPTION_INT_DIVIDE_BY_ZERO,         "Division durch Null (int)" },
        { EXCEPTION_FLT_OVERFLOW,               "Gleitkommaüberlauf" },
        { EXCEPTION_FLT_UNDERFLOW,              "Gleitkommaunterlauf" },
        { EXCEPTION_ILLEGAL_INSTRUCTION,        "Ungültige CPU-Instruktion" },
        { EXCEPTION_IN_PAGE_ERROR,              "Zugriffsfehler bei Paging" },
        { EXCEPTION_INT_OVERFLOW,               "Ganzzahlüberlauf (int)" },
        { EXCEPTION_INVALID_DISPOSITION,        "Ungültige Ausnahmebehandlung" },
        { EXCEPTION_NONCONTINUABLE_EXCEPTION,   "Nicht fortsetzbare Ausnahme" },
        { EXCEPTION_PRIV_INSTRUCTION,           "Privilegierte CPU-Instruktion" },
        { EXCEPTION_STACK_OVERFLOW,             "Stacküberlauf" },
        { EXCEPTION_GUARD_PAGE,                 "Zugriff auf GUARD-Seite" },
        { CONTROL_C_EXIT,                       "Strg+C oder Break" }
    };

    auto code = exception_info->ExceptionRecord->ExceptionCode;
    auto addr = exception_info->ExceptionRecord->ExceptionAddress;

    std::time_t now = std::time(nullptr);
    std::tm local_tm;
    localtime_s(&local_tm, &now);
    std::ostringstream timestamp_ss;
    timestamp_ss << std::put_time(&local_tm, "%Y-%m-%d_%H-%M-%S");
    std::string timestamp = timestamp_ss.str();

    char exe_path[MAX_PATH];
    GetModuleFileNameA(nullptr, exe_path, MAX_PATH);
    fs::path base_path = fs::path(exe_path).parent_path();
    fs::path crash_dir = base_path / "crash";
    fs::create_directories(crash_dir);

    std::string dump_file = (crash_dir / ("dump_" + timestamp + ".dmp")).string();
    std::string log_file = (crash_dir / "crash.log").string();

    HANDLE hFile = CreateFileA(dump_file.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = exception_info;
        mdei.ClientPointers = FALSE;

        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &mdei, nullptr, nullptr);
        CloseHandle(hFile);
    }

    std::string cause = "Unbekannte Ausnahme";
    auto it = exception_strings.find(code);
    if (it != exception_strings.end()) cause = it->second;

    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    SymInitialize(GetCurrentProcess(), NULL, TRUE);

    char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;

    std::string function_name = "[nicht auflösbar]";
    DWORD64 displacement = 0;
    if (SymFromAddr(GetCurrentProcess(), (DWORD64)addr, &displacement, pSymbol)) {
        function_name = std::string(pSymbol->Name) + " + 0x" + std::to_string(displacement);
    }

    IMAGEHLP_LINE64 line;
    DWORD lineDisp = 0;
    ZeroMemory(&line, sizeof(line));
    line.SizeOfStruct = sizeof(line);
    std::string file_info = "[nicht verfügbar]";
    if (SymGetLineFromAddr64(GetCurrentProcess(), (DWORD64)addr, &lineDisp, &line)) {
        file_info = std::string(line.FileName) + ":" + std::to_string(line.LineNumber);
    }

    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();

    std::ofstream log(log_file);
    if (log) {
        log << "[CRASH_HANDLER] Zeit: " << timestamp << "\n";
        log << "Exception Code: 0x" << std::hex << code << " (" << cause << ")\n";
        log << "Exception Addr: " << addr << "\n";
        log << "Funktion: " << function_name << "\n";
        log << "Quelle:   " << file_info << "\n";
        log << "Dump File: " << dump_file << "\n";
        log << "Prozess-ID: " << pid << "\n";
        log << "Thread-ID:  " << tid << "\n";
        log << "EXE-Pfad:    " << exe_path << "\n";
        log << "Arbeitsverz: " << fs::current_path().string() << "\n";

        if (exception_info && exception_info->ContextRecord) {
            const CONTEXT* ctx = exception_info->ContextRecord;
    #ifdef _M_X64
            log << "RIP: 0x" << std::hex << ctx->Rip << "\n";
            log << "RSP: 0x" << std::hex << ctx->Rsp << "\n";
            log << "RAX: 0x" << std::hex << ctx->Rax << "\n";
            log << "RCX: 0x" << std::hex << ctx->Rcx << "\n";
    #endif
        }

        log << "Erkannt in: crash_handler.cpp → global_exception_handler()\n";
        log.flush();
    }

    std::ostringstream msg;
    msg << "Crash wurde erkannt!\n\n"
        << "Typ: " << cause << "\n"
        << "Code: 0x" << std::hex << code << "\n"
        << "Adresse: " << addr << "\n\n"
        << "Funktion: " << function_name << "\n"
        << "Quelle:   " << file_info << "\n\n"
        << "Dump-Datei:\n" << dump_file << "\n"
        << "Log-Datei:\n" << log_file;

    MessageBoxA(nullptr, msg.str().c_str(), "Crash erkannt", MB_ICONERROR | MB_OK);

    ExitProcess(1);
    return EXCEPTION_EXECUTE_HANDLER;
}

void install_crash_handler() {
    SetUnhandledExceptionFilter(global_exception_handler);
}

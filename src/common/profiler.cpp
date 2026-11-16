#include "common/profiler.h"

#include "common/emulatorConfig.h"

#include <common/TracyProtocol.hpp>
#include <common/TracyVersion.hpp>
#include <cstdio>
#include <tracy/Tracy.hpp>

#if KYTY_PLATFORM == KYTY_PLATFORM_WINDOWS
// Minimal declarations (windows.h is not included here).
extern "C" __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int, unsigned long, const char*, int, wchar_t*, int);
extern "C" __declspec(dllimport) long __stdcall SetThreadDescription(void*, const wchar_t*);
#endif

namespace Profiler {

void SetThreadName(const char* name) {
#if KYTY_PLATFORM == KYTY_PLATFORM_WINDOWS
	// Also set the Windows thread description, readable by debuggers and profilers (GetThreadDescription).
	if (name != nullptr) {
		wchar_t wide[128] = {};
		::MultiByteToWideChar(65001 /* CP_UTF8 */, 0, name, -1, wide, 127);
		::SetThreadDescription(reinterpret_cast<void*>(-2) /* current thread */, wide);
	}
#endif
	if (tracy::ProfilerAvailable() && name != nullptr) {
		tracy::SetThreadName(name);
	}
}

void Initialize() {
	if (Config::ProfilerEnabled() && !tracy::ProfilerAvailable()) {
		tracy::StartupProfiler();
		TracySetProgramName("KytyPS5");
		::printf("Tracy profiler enabled: client %d.%d.%d, protocol %u, "
		         "broadcast %u, connect to 127.0.0.1:8086\n",
		         tracy::Version::Major, tracy::Version::Minor, tracy::Version::Patch,
		         tracy::ProtocolVersion, tracy::BroadcastVersion);
	}
}

void Shutdown() {
	if (tracy::ProfilerAvailable()) {
		tracy::ShutdownProfiler();
	}
}

} // namespace Profiler

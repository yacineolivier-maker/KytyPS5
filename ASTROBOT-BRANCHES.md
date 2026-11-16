# Astro Bot branches of this fork

> Prepared with AI assistance (Claude); built and tested by the repository owner as stated per branch; not submitted upstream.

This fork of [KytyPS5/KytyPS5](https://github.com/KytyPS5/KytyPS5) carries a few changes first written while running
ASTRO BOT (PS5) on a Windows PC with the Jetsku U59 fork of KytyPS5, adapted here to the official code. Base of every
branch: `41e2147c` ("graphics: render 4_4_4_4 targets as 5_5_5_1 on hosts that cannot render them"), the official
`main` when the fork was made. The default branch (`main`) is left unchanged.

Test machine of the original work: Intel Core i5-12400F (Intel host: the official build needs `--amd-cpu` there),
NVIDIA RTX 3060, Windows 11. No game data, keys or firmware are included in any branch; guest (eboot) addresses
quoted in comments are only locations in the game's code.

**None of these branches was run in the game on this official base.** They were each compiled on their own (Release,
clang-cl, Windows). The "evidence" below comes from the same changes on the Jetsku U59 base; the original versions
are on the fork <https://github.com/yacineolivier-maker/KytyPS5-Jetsku-AstroBot> (same branch names).

All switches are environment variables read once at start-up; with the switch unset the behaviour is the one of the
base.

## Summary

| Branch | Kind | Switch | Status on this base |
|---|---|---|---|
| `fix/esc-exits-fullscreen` | fix | none | compiled only, not tested in game on this base |
| `feature/app0-overlay-subst` | feature | `KYTY_APP0_OVERLAY`, `KYTY_APP0_SUBST` | compiled only, not tested in game on this base |
| `fix/windows-thread-names` | fix | none (Windows only) | compiled only, not tested in game on this base |
| `fix/amd-rsqrt-on-intel` | fix / speed | `KYTY_AMD_RSQRT=exact\|fast\|native` (default `exact`) | compiled only, not tested in game on this base |
| `diag/wait-graph` | diagnostic | `KYTY_WAIT_GRAPH=1` | compiled only, not tested in game on this base |

Not ported here:

- `fix/kernel-sema-targeted-wake`: already in the official code in another form (commit `a0268f1d`, "kernel: a
  semaphore signal wakes only the waiters it hands counts to": one condition variable per waiter).
- `diag/render-pass-stats`: it extends the `KYTY_CP_COMMIT_STATS` command-processor diagnostics
  (`drawPrep/commitStats.cpp`), which do not exist in the official code.
- `diag/frame-path`: it hooks parts of the Jetsku command processor and submission path (native command recorder,
  submission broker, deferred labels, sequenced submissions, frame fence) that the official code does not have; a
  port would be a rewrite, not an adaptation.

## `fix/esc-exits-fullscreen`

- **Goal**: F11 and Alt+Enter toggle desktop fullscreen; Escape now also returns to windowed mode (only when
  fullscreen, not on auto-repeat). The key is still forwarded to the game input as before.
- **Astro Bot problem**: with a controller-only setup there was no obvious key to leave fullscreen.
- **Evidence**: symptom; used daily on the Jetsku U59 base.
- **Switch**: none.
- **Status**: compiled only, not tested in game on this base.

## `feature/app0-overlay-subst`

- **Goal**: change small files of the game without modifying the dump.
  - `KYTY_APP0_OVERLAY=<folder>`: a file under `/app0` is served from this folder when a copy exists there.
  - `KYTY_APP0_SUBST=<rules file>`: same-length byte replacements, or a clamp of the number after a tag, applied in
    memory to data read from `/app0` files (`KernelRead`, `KernelPread`, AMPR host reads).
- **Astro Bot problem**: choosing the start level through the game's argument file, replacing button-prompt textures
  (Xbox A/B/X/Y instead of PlayStation symbols), testing lighter visual settings stored in text files.
- **Evidence**: used on the Jetsku U59 base (button prompts, level selection, shadow-distance clamp).
- **Switch**: `KYTY_APP0_OVERLAY`, `KYTY_APP0_SUBST`.
- **Status**: compiled only, not tested in game on this base.

## `fix/windows-thread-names`

- **Goal**: `Profiler::SetThreadName` (`KYTY_PROFILER_THREAD`) also sets the Windows thread description, and each guest
  thread gets its game-given name as Windows thread description, as Linux already does with `pthread_setname_np`.
  (On the Jetsku base the guest names are prefixed with `jeu:`; here they are plain, like on Linux.)
- **Astro Bot problem**: per-thread CPU measurements were impossible with anonymous thread ids.
- **Evidence**: used by the per-thread CPU sampling done on the Jetsku U59 base.
- **Switch**: none (Windows only).
- **Status**: compiled only, not tested in game on this base.

## `fix/amd-rsqrt-on-intel`

- **Goal**: on Intel hosts the AMD instruction patch replaces each 128-bit `VRSQRTPS` with an exact 1/sqrt in double
  precision (MXCSR saved and restored). `KYTY_AMD_RSQRT=fast` uses single-precision `VSQRTPS` + `VDIVPS` (a few ulp
  from exact, better than the ~12 bits AMD's `VRSQRTPS` guarantees); `KYTY_AMD_RSQRT=native` keeps Intel's own
  `VRSQRTPS`.
- **Astro Bot problem**: the game executes this instruction in hot code; the exact replacement cost frame rate on
  Intel.
- **Evidence** (Jetsku U59 base, 12 Nov 2026, 1080p, 30 fps mode): with `fast`, 11 of 24 main levels at 30 fps
  instead of 7; for example Aerial Garden 21.9 -> 23.4 fps, Pirate Island 26.1 -> 29.4.
- **Switch**: `KYTY_AMD_RSQRT=exact|fast|native` (default `exact` = unchanged).
- **Status**: compiled only, not tested in game on this base.

## `diag/wait-graph`

- **Goal**: `KYTY_WAIT_GRAPH=1` times the blocking waits of guest threads (kernel and POSIX semaphores, pthread
  condition variables and contended mutexes, event flags, event queues, the `sceAgcSuspendPoint` wait, sleeps), names
  the waker of each wait from the last signal of the same object, counts yield loops by guest address, and prints
  every 10 s the main thread and the three other guest threads that wait most, with the "A waits for B waits for C"
  chains (report in French). Here the frame wait hooked is `GuestGpu::SuspendPoint` (the acquire of
  `m_suspend_point_ready`, released by the GPU thread).
- **Astro Bot problem**: find what the game's main thread waits for.
- **Evidence** (Jetsku U59 base, 16 Nov 2026, Aerial Garden): the main thread waits only ~8 ms per frame and computes
  ~82 % of the time; the largest wait is ~1,300 semaphore round trips per frame with one helper thread.
- **Switch**: `KYTY_WAIT_GRAPH=1` (off: each hook is one inline flag test).
- **Status**: compiled only, not tested in game on this base.

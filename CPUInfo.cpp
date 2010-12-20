// Copyright (c) 2005 Chad Austin
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
// Logic contained in this software gleaned from the following resources:
// 
// Intel Software Development Manuals
//   http://www.intel.com/design/itanium/manuals/245319.pdf
//
// AMD Processor Detection
//   http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/20734.pdf
//   http://www.amd.com/us-en/assets/content_type/DownloadableAssets/dwamd_24593.pdf
//   http://www.amd.com/us-en/assets/content_type/DownloadableAssets/dwamd_24594.pdf
//
// AMD64 Instruction Set Reference
//
// sandpile.org
// http://www.sandpile.org/post/msgs/20003246.htm
// http://www.sandpile.org/ia32/cpuid.htm
//
// Iain Chesworth @ codeproject
//   http://www.codetools.com/system/camel.asp
//
// Rob Wyatt @ gamasutra:
//   http://www.gamasutra.com/features/wyatts_world/19990709/processor_detection_01.htm
//
// http://grafi.ii.pw.edu.pl/gbm/x86/cpuid.html


#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "CPUInfo.h"


#if defined(_MSC_VER) || defined(__CYGWIN__)

#include <windows.h>

typedef DWORD     u32;
typedef ULONGLONG u64;

#else

typedef unsigned long      u32;
typedef unsigned long long u64;

#endif


const char* CPUInfo::getVendorName() const {
    // Return the vendor ID.
    switch (identity.manufacturer) {
        case Intel:     return "Intel Corporation";
        case AMD:       return "Advanced Micro Devices";
        case NSC:       return "National Semiconductor";
        case Cyrix:     return "Cyrix Corp., VIA Inc.";
        case NexGen:    return "NexGen Inc., Advanced Micro Devices";
        case IDT:       return "IDT\\Centaur, Via Inc.";
        case UMC:       return "United Microelectronics Corp.";
        case Rise:      return "Rise";
        case Transmeta: return "Transmeta";
        default:        return identity.vendor;
    }
}


std::string CPUInfo::getProcessorName() const {
    if (identity.hasExtendedName) {
        return identity.extendedName + identity.firstNonSpace;
    } else if (identity.brand != 0) {
        return getProcessorBrandName();
    } else {
        return getClassicalProcessorName();
    }
}


const char* CPUInfo::getProcessorTypeName() const {
    switch (identity.type) {
        case 0:  return "Original OEM Processor";
        case 1:  return "Intel OverDrive Processor";
        case 2:  return "Dual Processor";
        case 3:  return "Reserved";
        default: return "Other";
    }
}


std::string CPUInfo::getProcessorBrandName() const {
    // http://sandpile.org/ia32/cpuid.htm and IA-32 Manual 2A
    switch (identity.brand) {
        case 0x00: return "Not Supported";
        case 0x01: return "0.18 µm Intel Celeron";
        case 0x02: return "0.18 µm Intel Pentium III";
        case 0x03: return "0.18 µm Intel Pentium III Xeon"
                          " -OR- "
                          "0.13 µm Intel Celeron";
        case 0x04: return "0.13 µm Intel Pentium III";
        case 0x06: return "0.13 µm Intel Pentium III mobile";
        case 0x07: return "0.13 µm Intel Celeron mobile";
        case 0x08: return "0.18 µm Intel Pentium 4"
                          " -OR- "
                          "0.13 µm Intel Celeron 4 mobile (0F24h)";
        case 0x09: return "0.13 µm Intel Pentium 4";
        case 0x0A: return "0.18 µm Intel Celeron 4";
        case 0x0B: return "0.18 µm Intel Pentium 4 Xeon MP"
                          " -OR- "
                          "0.13 µm Intel Pentium 4 Xeon";
        case 0x0C: return "0.13 µm Intel Pentium 4 Xeon MP";
        case 0x0E: return "0.18 µm Intel Pentium 4 Xeon"
                          " -OR- "
                          "0.13 µm Intel Pentium 4 mobile (production)";
        case 0x0F: return "0.13 µm Intel Celeron 4 mobile (0F27h)"
                          " -OR- "
                          "0.13 µm Intel Pentium 4 mobile (samples)";
        case 0x11: return "Mobile Genuine Intel processor";
        case 0x12: return "0.13 µm Intel Celeron M"
                          " -OR- "
                          "0.09 µm Intel Celeron M";
        case 0x13: return "Mobile Intel Celeron processor";
        case 0x14: return "Intel Celeron processor";
        case 0x15: return "Mobile Genuine Intel processor";
        case 0x16: return "0.13 µm Intel Pentium M"
                          " -OR- "
                          "0.09 µm Intel Pentium M";
        case 0x17: return "Mobile Intel Celeron processor";
    }

    char buffer[100];

    int top3    = (identity.brand >> 5) & 7;
    int bottom5 = identity.brand & 31;
    switch (top3) {
        case 0: sprintf(buffer, "Engineering Sample %d", bottom5);           return buffer;
        case 1: sprintf(buffer, "AMD Athlon 64 %d00+", 22 + bottom5);        return buffer;
        case 2: sprintf(buffer, "AMD Athlon 64 %d00+ mobile", 22 + bottom5); return buffer;
        case 3: sprintf(buffer, "AMD Opteron UP 1%d", 38 + 2 * bottom5);     return buffer;
        case 4: sprintf(buffer, "AMD Opteron DP 2%d", 38 + 2 * bottom5);     return buffer;
        case 5: sprintf(buffer, "AMD Opteron MP 8%d", 38 + 2 * bottom5);     return buffer;
        // n/a #6   AMD Athlon 64 FX-ZZ (ZZ=24+xxxxxb)
        // The upper 3 bits aren't sufficient to encode a value of 9=1001b.
        // Thus the Athlon 64 FX requires the 12-bit brand ID.
    }

    return "Unknown";
}


const char* CPUInfo::getClassicalProcessorName() const {
    switch (identity.manufacturer) {
        case Intel:
            switch (identity.family) {
                case 3: return "Newer i80386 family";
                case 4:
                    switch (identity.model) {
                        case 0:  return "i80486DX-25/33";
                        case 1:  return "i80486DX-50";
                        case 2:  return "i80486SX";
                        case 3:  return "i80486DX2";
                        case 4:  return "i80486SL";
                        case 5:  return "i80486SX2";
                        case 7:  return "i80486DX2 WriteBack";
                        case 8:  return "i80486DX4";
                        case 9:  return "i80486DX4 WriteBack";
                        default: return "Unknown 80486 family";
                    }
                case 5:
                    switch (identity.model) {
                        case 0:  return "P5 A-Step";
                        case 1:  return "P5";
                        case 2:  return "P54C";
                        case 3:  return "P24T OverDrive";
                        case 4:  return "P55C";
                        case 7:  return "P54C";
                        case 8:  return "P55C (0.25µm)";
                        default: return "Unknown Pentium® family";
                    }
                case 6:
                    switch (identity.model) {
                        case 0:   return "P6 A-Step";
                        case 1:   return "P6";
                        case 3:   return "Pentium® II (0.28 µm)";
                        case 5:   return "Pentium® II (0.25 µm)";
                        case 6:   return "Pentium® II With On-Die L2 Cache";
                        case 7:   return "Pentium® III (0.25 µm)";
                        case 8:   return "Pentium® III (0.18 µm) With 256 KB On-Die L2 Cache";
                        case 9:   return "Pentium® M or Celeron M";
                        case 0xa: return "Pentium® III (0.18 µm) With 1 Or 2 MB On-Die L2 Cache";
                        case 0xb: return "Pentium® III (0.13 µm) With 256 Or 512 KB On-Die L2 Cache";
                        case 0xd: return "Pentium M (90 nm)";
                        case 0xe: return "Core Solo or Duo (65 nm)";
                        case 0xf: return "Core 2 Solo or Duo (65 nm)";
                        default:  return "Unknown P6 family";
                    }
                case 7: return "Intel Merced (IA-64)";
                case 15:
                    switch (identity.model) {
                        case 0:  return "Pentium® IV (0.18 µm)";
                        case 1:  return "Pentium® IV (0.18 µm)";
                        case 2:  return "Pentium® IV (0.13 µm)";
                        default: return "Unknown Pentium 4 family";
                    }
                case 31: return "Intel McKinley (IA-64)";
                default: return "Unknown Intel family";
            }

        case AMD:
            switch (identity.family) {
                case 4:
                    switch (identity.model) {
                        case 3:   return "80486DX2";
                        case 7:   return "80486DX2 WriteBack";
                        case 8:   return "80486DX4";
                        case 9:   return "80486DX4 WriteBack";
                        case 0xe: return "5x86";
                        case 0xf: return "5x86WB";
                        default:  return "Unknown 80486 family";
                    }
                case 5:
                    switch (identity.model) {
                        case 0:   return "SSA5 (PR75, PR90, PR100)";
                        case 1:   return "5k86 (PR120, PR133)";
                        case 2:   return "5k86 (PR166)";
                        case 3:   return "5k86 (PR200)";
                        case 6:   return "K6 (0.30 µm)";
                        case 7:   return "K6 (0.25 µm)";
                        case 8:   return "K6-2";
                        case 9:   return "K6-III";
                        case 0xd: return "K6-2+ or K6-III+ (0.18 µm)";
                        default:  return "Unknown 80586 family";
                    }
                case 6:
                    switch (identity.model) {
                        case 1:  return "Athlon™ (0.25 µm)";
                        case 2:  return "Athlon™ (0.18 µm)";
                        case 3:  return "Duron™ (SF core)";
                        case 4:  return "Athlon™ (Thunderbird core)";
                        case 6:  return "Athlon™ (Palomino core)";
                        case 7:  return "Duron™ (Morgan core)";
                        case 8:  return (features.supportsMP ?
                                    "Athlon™ MP (Thoroughbred core)" :
                                    "Athlon™ XP (Thoroughbred core)");
                        default: return "Unknown K7 family";
                    }
                default:
                    return "Unknown AMD family";
            }

        case Transmeta:
            switch (identity.family) {    
                case 5:
                    switch (identity.model) {
                        case 4:  return "Crusoe TM3x00 and TM5x00";
                        default: return "Unknown Crusoe family";
                    }
                default:
                    return "Unknown Transmeta family";
            }

        case Rise:
            switch (identity.family) {    
                case 5:
                    switch (identity.model) {
                        case 0:  return "mP6 (0.25 µm)";
                        case 2:  return "mP6 (0.18 µm)";
                        default: return "Unknown Rise family";
                    }
                default:
                    return "Unknown Rise family";
            }

        case UMC:
            switch (identity.family) {    
                case 4:
                    switch (identity.model) {
                        case 1:  return "U5D";
                        case 2:  return "U5S";
                        default: return "Unknown UMC family";
                    }
                default:
                    return "Unknown UMC family";
            }

        case IDT:
            switch (identity.family) {    
                case 5:
                    switch (identity.model) {
                        case 4:  return "C6";
                        case 8:  return "C2";
                        case 9:  return "C3";
                        default: return "Unknown IDT\\Centaur family";
                    }
                    break;
                case 6:
                    switch (identity.model) {
                        case 6:  return "VIA Cyrix III - Samuel";
                        default: return "Unknown IDT\\Centaur family";
                    }
                default:
                    return "Unknown IDT\\Centaur family";
            }

        case Cyrix:
            switch (identity.family) {    
                case 4:
                    switch (identity.model) {
                        case 4:  return "MediaGX GX, GXm";
                        case 9:  return "5x86";
                        default: return "Unknown Cx5x86 family";
                    }
                case 5:
                    switch (identity.model) {
                        case 2:  return "Cx6x86";
                        case 4:  return "MediaGX GXm";
                        default: return "Unknown Cx6x86 family";
                    }
                case 6:
                    switch (identity.model) {
                        case 0:  return "6x86MX";
                        case 5:  return "Cyrix M2 Core";
                        case 6:  return "WinChip C5A Core";
                        case 7:  return "WinChip C5B\\C5C Core";
                        case 8:  return "WinChip C5C-T Core";
                        default: return "Unknown 6x86MX\\Cyrix III family";
                    }
                default:
                    return "Unknown Cyrix family";
            }

        case NexGen:
            switch (identity.family) {    
                case 5:
                    switch (identity.model) {
                        case 0:  return "Nx586 or Nx586FPU";
                        default: return "Unknown NexGen family";
                    }
                default:
                    return "Unknown NexGen family";
            }

        case NSC:
            return "Cx486SLC \\ DLC \\ Cx486S A-Step";

        default:
            return "Unknown manufacturer.";
    }
}


#ifdef _MSC_VER  // Use SEH on Win32.


#if 0

static bool getCPUIDSupport() {
    // The "right" way, which doesn't work under certain Windows versions.
    // Courtesy of Iain Chesworth.

    int present = 0;
    __asm {
        pushfd                      ; save EFLAGS to stack.
        pop     eax                 ; store EFLAGS in eax.
        mov     edx, eax            ; save in edx for testing later.
        xor     eax, 0200000h       ; switch bit 21.
        push    eax                 ; copy "changed" value to stack.
        popfd                       ; save "changed" eax to EFLAGS.
        pushfd
        pop     eax
        xor     eax, edx            ; See if bit changeable.
        jz      cpuid_not_present
        mov     present, 1          ; CPUID present - enable its usage

    cpuid_not_present:
    }

    return present != 0;
}

#endif


static bool getCPUIDSupport() {
    __try {
        __asm {
            mov eax, 0
            cpuid
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
    return true;
}


static bool getSSEFPSupport() {
    __try {
        __asm {
            orps xmm0, xmm0
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
    return true;
}


static void classicalTimingLoop(u32 loopLength) {
    __asm {
        mov eax, 0x80000000
        mov ebx, loopLength
    timingLoop:
        bsf ecx, eax
        dec ebx
        jnz timingLoop
    }
}


#else  // Assume platform that raises signals for invalid instructions.


#include <setjmp.h>
#include <signal.h>

#ifdef __CYGWIN__
#define __thread __declspec(thread)
#endif

typedef void (*SignalHandler)(int);

static __thread jmp_buf check_jmpbuf;

static void handleSignal(int) {
    longjmp(check_jmpbuf, 1);
}

static bool getCPUIDSupport() {
    SignalHandler oldSIGILL  = signal(SIGILL,  handleSignal);
    SignalHandler oldSIGSEGV = signal(SIGSEGV, handleSignal);

    bool hasCPUID = false;
    if (setjmp(check_jmpbuf) == 0) {
        asm("movl $0, %eax\n"
            "cpuid\n");
        hasCPUID = true;
    }

    signal(SIGILL,  oldSIGILL);
    signal(SIGSEGV, oldSIGSEGV);

    return hasCPUID;
}


static bool getSSEFPSupport() {
    SignalHandler oldSIGILL  = signal(SIGILL,  handleSignal);
    SignalHandler oldSIGSEGV = signal(SIGSEGV, handleSignal);

    bool hasSSEFP = false;
    if (setjmp(check_jmpbuf) == 0) {
        asm("orps %xmm0, %xmm0\n");
        hasSSEFP = true;
    }

    signal(SIGILL,  oldSIGILL);
    signal(SIGSEGV, oldSIGSEGV);

    return hasSSEFP;
}


static void classicalTimingLoop(u32 loopLength) {
    asm("mov $0x80000000, %%eax\n"
        "timingLoop:\n"
        "bsf %%eax, %%ecx\n"
        "dec %%ebx\n"
        "jnz timingLoop\n"
        :
        : "b" (loopLength)
        : "%eax", "%ecx");
}


#endif


#ifdef _MSC_VER

static void CPUID(u32 level, u32* eax, u32* ebx, u32* ecx, u32* edx) {
    assert(getCPUIDSupport());

    u32 _eax, _ebx, _ecx, _edx;
    __asm {
        mov eax, level
        cpuid
        mov _eax, eax
        mov _ebx, ebx
        mov _ecx, ecx
        mov _edx, edx
    }

    if (eax) *eax = _eax;
    if (ebx) *ebx = _ebx;
    if (ecx) *ecx = _ecx;
    if (edx) *edx = _edx;
}

static u64 RDTSC() {
    u32 h, l;
    __asm {
        rdtsc
        mov h, edx
        mov l, eax
    }
    return (u64(h) << 32) + l;
}

#else

static void CPUID(u32 level, u32* eax, u32* ebx, u32* ecx, u32* edx) {
    assert(getCPUIDSupport());

    u32 _eax, _ebx, _ecx, _edx;
    asm("cpuid"
        : "=a" (_eax), "=b" (_ebx), "=c" (_ecx), "=d" (_edx)
        : "a" (level));

    if (eax) *eax = _eax;
    if (ebx) *ebx = _ebx;
    if (ecx) *ecx = _ecx;
    if (edx) *edx = _edx;
}

static u64 RDTSC() {
    u32 eax, edx;
    asm("rdtsc"
        : "=a" (eax), "=d" (edx));
    return (u64(edx) << 32) + eax;
}

#endif


#if defined(_MSC_VER) || defined(__CYGWIN__)

static u64 getHPFrequency() {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}

static u64 getHPCounter() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

#else

#include <sys/time.h>

static u64 getHPFrequency() {
    return 1000000;
}

static u64 getHPCounter() {
    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

#endif


static bool checkExtendedLevelSupport(const CPUInfo::Identity& id, u32 levelToCheck) {
    // The way everyone else checks is to see if the result of running with
    // input 0x80000000 is greater than or equal to 0x80000000.  The Intel
    // docs indicate that this may not always be the case.

    // The extended CPUID is supported by various vendors starting with the
    // following CPU models:
    //
    //      Manufacturer & Chip Name        | Family   Model   Revision
    //
    //      AMD K6, K6-2                    |    5       6         x
    //      Cyrix GXm, Cyrix III "Joshua"   |    5       4         x
    //      IDT C6-2                        |    5       8         x
    //      VIA Cyrix III                   |    6       5         x
    //      Transmeta Crusoe                |    5       x         x
    //      Intel Pentium 4                 |    f       x         x
    //

    // We check to see if a supported processor is present...
    if (id.manufacturer == CPUInfo::AMD) {
        if (id.family < 5) return false;
        if (id.family == 5 && id.model < 6) return false;
    } else if (id.manufacturer == CPUInfo::Cyrix) {
        if (id.family < 5) return false;
        if (id.family == 5 && id.model < 4) return false;
        if (id.family == 6 && id.model < 5) return false;
    } else if (id.manufacturer == CPUInfo::IDT) {
        if (id.family < 5) return false;
        if (id.family == 5 && id.model < 8) return false;
    } else if (id.manufacturer == CPUInfo::Transmeta) {
        if (id.family < 5) return false;
    } else if (id.manufacturer == CPUInfo::Intel) {
        if (id.family < 0xF) return false;
    }

    u32 maxExtendedLevel = 0;
    CPUID(0x80000000, &maxExtendedLevel, NULL, NULL, NULL);
    return maxExtendedLevel >= levelToCheck;
}


static void getIdentity(CPUInfo::Identity& id) {
    CPUID(0, NULL,
          (u32*)id.vendor,
          (u32*)(id.vendor + 8),
          (u32*)(id.vendor + 4));
    id.vendor[12] = 0;


    if      (memcmp(id.vendor, "GenuineIntel", 12) == 0) id.manufacturer = CPUInfo::Intel;     // Intel Corp.
    else if (memcmp(id.vendor, "UMC UMC UMC ", 12) == 0) id.manufacturer = CPUInfo::UMC;       // United Microelectronics Corp.
    else if (memcmp(id.vendor, "AuthenticAMD", 12) == 0) id.manufacturer = CPUInfo::AMD;       // Advanced Micro Devices
    else if (memcmp(id.vendor, "AMD ISBETTER", 12) == 0) id.manufacturer = CPUInfo::AMD;       // Advanced Micro Devices (1994)
    else if (memcmp(id.vendor, "CyrixInstead", 12) == 0) id.manufacturer = CPUInfo::Cyrix;     // Cyrix Corp., VIA Inc.
    else if (memcmp(id.vendor, "NexGenDriven", 12) == 0) id.manufacturer = CPUInfo::NexGen;    // NexGen Inc. (now AMD)
    else if (memcmp(id.vendor, "CentaurHauls", 12) == 0) id.manufacturer = CPUInfo::IDT;       // IDT/Centaur (now VIA)
    else if (memcmp(id.vendor, "RiseRiseRise", 12) == 0) id.manufacturer = CPUInfo::Rise;      // Rise
    else if (memcmp(id.vendor, "GenuineTMx86", 12) == 0) id.manufacturer = CPUInfo::Transmeta; // Transmeta
    else if (memcmp(id.vendor, "TransmetaCPU", 12) == 0) id.manufacturer = CPUInfo::Transmeta; // Transmeta
    else if (memcmp(id.vendor, "Geode By NSC", 12) == 0) id.manufacturer = CPUInfo::NSC;       // National Semiconductor
    else                                                 id.manufacturer = CPUInfo::UnknownManufacturer;


    u32 signature_eax, signature_ebx;
    CPUID(1, &signature_eax, &signature_ebx, NULL, NULL);

    unsigned family    = (signature_eax >> 8)  & 0xF;
    unsigned ex_family = (signature_eax >> 20) & 0xFF;

    unsigned model    = (signature_eax >> 4)  & 0xF;
    unsigned ex_model = (signature_eax >> 16) & 0xF;

    id.type     = (signature_eax >> 12) & 0x3;
    id.family   = (ex_family << 4) + family;
    id.model    = (ex_model << 4) + model;
    id.stepping = signature_eax & 0xF;

    id.brand    = signature_ebx & 0xFF;
}


static void getExtendedIdentity(CPUInfo::Identity& id) {
    id.hasExtendedName = false;

    // Make sure this check is supported.
    if (!checkExtendedLevelSupport(id, 0x80000002)) return;
    if (!checkExtendedLevelSupport(id, 0x80000003)) return;
    if (!checkExtendedLevelSupport(id, 0x80000004)) return;

    CPUID(0x80000002,
          (u32*)id.extendedName,
          (u32*)id.extendedName + 1,
          (u32*)id.extendedName + 2,
          (u32*)id.extendedName + 3);
    CPUID(0x80000003,
          (u32*)id.extendedName + 4,
          (u32*)id.extendedName + 5,
          (u32*)id.extendedName + 6,
          (u32*)id.extendedName + 7);
    CPUID(0x80000004,
          (u32*)id.extendedName + 8,
          (u32*)id.extendedName + 9,
          (u32*)id.extendedName + 10,
          (u32*)id.extendedName + 11);
    id.extendedName[48] = 0;

    // Trim leading whitespace.
    unsigned firstNonSpace = 0;
    while (firstNonSpace < 48 && isspace(id.extendedName[firstNonSpace])) {
        ++firstNonSpace;
    }

    id.hasExtendedName = true;
    id.firstNonSpace = firstNonSpace;
}


static bool isBitSet(unsigned word, int bit) {
    return (word & (1 << bit)) != 0;
}


static void getFeatures(CPUInfo::Features& features) {
    u32 features_ebx;
    u32 features_ecx;
    u32 features_edx;
    CPUID(1, NULL, &features_ebx, &features_ecx, &features_edx);

#define F(name, bit) features.name = isBitSet(features_edx, (bit))

    F(fpu,     0);
    F(vme,     1);
    F(de,      2);
    F(pse,     3);
    F(tsc,     4);
    F(msr,     5);
    F(pae,     6);
    F(mce,     7);
    F(cx8,     8);
    F(apic,    9);
    F(sep,     11);
    F(mtrr,    12);
    F(pge,     13);
    F(mca,     14);
    F(cmov,    15);
    F(pat,     16);
    F(pse36,   17);
    F(serial,  18);
    F(clfsh,   19);
    F(ds,      21);
    F(acpi,    22);
    F(mmx,     23);
    F(fxsr,    24);
    F(sse,     25);
    F(sse2,    26);
    F(ss,      27);
    F(htt,     28);
    F(thermal, 29);
    F(ia64,    30);
    F(pbe,     31);

#undef F

    // Verify that floating point SSE works.
    if (features.sse) {
        features.ssefp = getSSEFPSupport();
    } else {
        features.ssefp = false;
    }

    features.CLFLUSHCacheLineSize = (features_ebx >> 8)  & 0xFF;
    features.APIC_ID              = (features_ebx >> 24) & 0xFF;

    features.sse3    = isBitSet(features_ecx, 0);
    features.monitor = isBitSet(features_ecx, 3);
    features.ds_cpl  = isBitSet(features_ecx, 4);
    features.est     = isBitSet(features_ecx, 7);
    features.tm2     = isBitSet(features_ecx, 8);
    features.cnxt_id = isBitSet(features_ecx, 10);

    features.logicalProcessorsPerPhysical = (features.htt
        ? (features_ebx >> 16) & 0xFF
        : 1);
}


static void getExtendedFeatures(const CPUInfo::Identity& id, CPUInfo::Features& features) {
    if (checkExtendedLevelSupport(id, 0x80000001)) {
        u32 ex_signature;
        u32 ex_features;
        CPUID(0x80000001, &ex_signature, NULL, NULL, &ex_features);

        // Retrieve the extended features of CPU present.
        features._3dnow     = isBitSet(ex_features, 31);
        features._3dnowPlus = isBitSet(ex_features, 30);
        features.ssemmx     = isBitSet(ex_features, 22);
        features.supportsMP = isBitSet(ex_features, 19);

        // MMX+ is reported differently by manufacturers.
        if (id.manufacturer == CPUInfo::AMD) {
            features.mmxPlus = isBitSet(ex_features, 22);
        } else if (id.manufacturer == CPUInfo::Cyrix) {
            features.mmxPlus = isBitSet(ex_features, 24);
        } else {
            features.mmxPlus = false;
        }
    } else {
        features._3dnow     = false;
        features._3dnowPlus = false;
        features.ssemmx     = false;
        features.mmxPlus    = false;
        features.supportsMP = false;
    }
}


static void getSerialNumber(CPUInfo& info) {
    // Verify that the processor has a serial number.
    assert(info.features.serial);

    unsigned char serialNumber[12];
    CPUID(3, NULL,
          (u32*)serialNumber,
          (u32*)serialNumber + 1,
          (u32*)serialNumber + 2);

    sprintf(info.features.serialNumber,
            "%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x",
            serialNumber[0],
            serialNumber[1],
            serialNumber[2],
            serialNumber[3],
            serialNumber[4],
            serialNumber[5],
            serialNumber[6],
            serialNumber[7],
            serialNumber[8],
            serialNumber[9],
            serialNumber[10],
            serialNumber[11]);
}


static bool getCacheDetails(const CPUInfo::Identity& id, CPUInfo::Cache& cache) {
    if (checkExtendedLevelSupport(id, 0x80000005)) {
        u32 L1[4];
        CPUID(0x80000005, &L1[0], &L1[1], &L1[2], &L1[3]);
        cache.L1CacheSize  = (L1[2] >> 24) & 0xFF;
        cache.L1CacheSize += (L1[3] >> 24) & 0xFF;
    } else {
        cache.L1CacheSize = -1;
    }

    if (checkExtendedLevelSupport(id, 0x80000006)) {
        u32 L2[4];
        CPUID(0x80000006, &L2[0], &L2[1], &L2[2], &L2[3]);
        cache.L2CacheSize = (L2[2] >> 16) & 0xFFFF;
    } else {
        cache.L2CacheSize = -1;
    }

    // No way to query L3.
    cache.L3CacheSize = -1;

    // Return failure if we cannot detect either cache with this method.
    return cache.L1CacheSize != -1 ||
           cache.L2CacheSize != -1;
}


static void getClassicalCacheDetails(CPUInfo::Cache& cache) {
    //int TLBCode   = -1;
    //int TLBData   = -1;
    int L1Code    = -1;
    int L1Data    = -1;
    int L1Trace   = -1;
    int L2Unified = -1;
    int L3Unified = -1;

    int passTotal;
    int passCounter = 0;
    do {
        unsigned char cacheData[16];
        CPUID(2,
              (u32*)cacheData,
              (u32*)cacheData + 1,
              (u32*)cacheData + 2,
              (u32*)cacheData + 3);

        passTotal = cacheData[0];
        for (int counter = 1; counter < 16; ++counter) {

            // Process descriptors.
            switch (cacheData[counter]) {
                case 0x00: break;
                //case 0x01: TLBCode = 4;         break;
                //case 0x02: TLBCode = 4096;      break;
                //case 0x03: TLBData = 4;         break;
                //case 0x04: TLBData = 4096;      break;
                case 0x06: L1Code = 8;          break;
                case 0x08: L1Code = 16;         break;
                case 0x0a: L1Data = 8;          break;
                case 0x0c: L1Data = 16;         break;
                case 0x10: L1Data = 16;         break;      // <-- FIXME: IA-64 Only
                case 0x15: L1Code = 16;         break;      // <-- FIXME: IA-64 Only
                case 0x1a: L2Unified = 96;      break;      // <-- FIXME: IA-64 Only
                case 0x22: L3Unified = 512;     break;
                case 0x23: L3Unified = 1024;    break;
                case 0x25: L3Unified = 2048;    break;
                case 0x29: L3Unified = 4096;    break;
                case 0x39: L2Unified = 128;     break;
                case 0x3c: L2Unified = 256;     break;
                case 0x40: L2Unified = 0;       break;      // <-- FIXME: No integrated L2 cache (P6 core) or L3 cache (P4 core).
                case 0x41: L2Unified = 128;     break;
                case 0x42: L2Unified = 256;     break;
                case 0x43: L2Unified = 512;     break;
                case 0x44: L2Unified = 1024;    break;
                case 0x45: L2Unified = 2048;    break;
                //case 0x50: TLBCode = 4096;      break;
                //case 0x51: TLBCode = 4096;      break;
                //case 0x52: TLBCode = 4096;      break;
                //case 0x5b: TLBData = 4096;      break;
                //case 0x5c: TLBData = 4096;      break;
                //case 0x5d: TLBData = 4096;      break;
                case 0x66: L1Data = 8;          break;
                case 0x67: L1Data = 16;         break;
                case 0x68: L1Data = 32;         break;
                case 0x70: L1Trace = 12;        break;
                case 0x71: L1Trace = 16;        break;
                case 0x72: L1Trace = 32;        break;
                case 0x77: L1Code = 16;         break;      // <-- FIXME: IA-64 Only
                case 0x79: L2Unified = 128;     break;
                case 0x7a: L2Unified = 256;     break;
                case 0x7b: L2Unified = 512;     break;
                case 0x7c: L2Unified = 1024;    break;
                case 0x7e: L2Unified = 256;     break;
                case 0x81: L2Unified = 128;     break;
                case 0x82: L2Unified = 256;     break;
                case 0x83: L2Unified = 512;     break;
                case 0x84: L2Unified = 1024;    break;
                case 0x85: L2Unified = 2048;    break;
                case 0x88: L3Unified = 2048;    break;      // <-- FIXME: IA-64 Only
                case 0x89: L3Unified = 4096;    break;      // <-- FIXME: IA-64 Only
                case 0x8a: L3Unified = 8192;    break;      // <-- FIXME: IA-64 Only
                case 0x8d: L3Unified = 3096;    break;      // <-- FIXME: IA-64 Only
                //case 0x90: TLBCode, 262144;     break;      // <-- FIXME: IA-64 Only
                //case 0x96: TLBCode, 262144;     break;      // <-- FIXME: IA-64 Only
                //case 0x9b: TLBCode, 262144;     break;      // <-- FIXME: IA-64 Only
            }
        }

        ++passCounter;

    } while (passCounter < passTotal);

    if (L1Code == -1 && L1Data == -1 && L1Trace == -1) {
        cache.L1CacheSize = -1;
    } else {
        cache.L1CacheSize = 0;
        if (L1Code  != -1) cache.L1CacheSize += L1Code;
        if (L1Data  != -1) cache.L1CacheSize += L1Data;
        if (L1Trace != -1) cache.L1CacheSize += L1Trace;
    }

    cache.L2CacheSize = L2Unified;
    cache.L3CacheSize = L3Unified;
}


static void getPowerManagement(const CPUInfo::Identity& id, CPUInfo::PowerManagement& pm) {
    if (checkExtendedLevelSupport(id, 0x80000007)) {
        u32 pmflags = 0;
        CPUID(0x80000007, NULL, NULL, NULL, &pmflags);

        pm.ts  = isBitSet(pmflags, 0);
        pm.fid = isBitSet(pmflags, 1);
        pm.vid = isBitSet(pmflags, 2);
        pm.ttp = isBitSet(pmflags, 3);
        pm.tm  = isBitSet(pmflags, 4);
        pm.stc = isBitSet(pmflags, 5);
    } else {
        pm.ts  = false;
        pm.fid = false;
        pm.vid = false;
        pm.ttp = false;
        pm.tm  = false;
        pm.stc = false;
    }
}


/// Duration in milliseconds.
static int measureFrequency(unsigned duration) {
    // Run a high-performance timer with a known frequency against the
    // processor clock to calculate the processor's frequency.

    u64 frequencyPC = getHPFrequency();
    u64 ticks = duration * frequencyPC / 1000;

    u64 startPC  = getHPCounter();
    u64 startTSC = RDTSC();

    u64 endPC;
    do {
        endPC = getHPCounter();
    } while ((endPC - startPC) < ticks);

    u64 endTSC = RDTSC();

    u64 elapsedPC  = endPC  - startPC;
    u64 elapsedTSC = endTSC - startTSC;

    return int(elapsedTSC * frequencyPC / elapsedPC / 1000000);
}


static int getFrequency() {
    return measureFrequency(50);
}


static int getClassicalFrequency(const CPUInfo& info) {
    static const u32 LOOP_LENGTH = 10000000;

    u64 start = getHPCounter();
    classicalTimingLoop(LOOP_LENGTH);
    u64 end   = getHPCounter();

    u64 countsPerSecond = getHPFrequency();

    // Calculate loop running time in seconds.
    double duration = double(end - start) / countsPerSecond;

    // Calculate the clock speed.
    if (info.identity.family == 3) {
        // 80386 processors....  Loop time is 115 cycles!
        return int(((LOOP_LENGTH * 115.0) / duration) / 1000000);
    } else if (info.identity.family == 4) {
        // 80486 processors....  Loop time is 47 cycles!
        return int(((LOOP_LENGTH * 47.0) / duration) / 1000000);
    } else if (info.identity.family == 5) {
        // Pentium processors....  Loop time is 43 cycles!
        return int(((LOOP_LENGTH * 43.0) / duration) / 1000000);
    } else {
        // Unknown cycle count.
        return 0;
    }
}


static int getCPUFrequency(const CPUInfo& info) {
    if (info.features.tsc) {
        return getFrequency();
    } else {
        return getClassicalFrequency(info);
    }
}


void getCPUInfo(CPUInfo& info) {
    // CPUID support.
    info.supportsCPUID = getCPUIDSupport();

    if (info.supportsCPUID) {
        // Identity.
        getIdentity(info.identity);
        getExtendedIdentity(info.identity);

        // Features.
        getFeatures(info.features);
        getExtendedFeatures(info.identity, info.features);
        if (info.features.serial) {
            getSerialNumber(info);
        }

        // Cache.
        if (!getCacheDetails(info.identity, info.cache)) {
            getClassicalCacheDetails(info.cache);
        }

        // Power management.
        getPowerManagement(info.identity, info.powerManagement);

        info.frequency = getCPUFrequency(info);
    }
}


#if defined(_MSC_VER) || defined(__CYGWIN__)

int getCPUCount() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
}

static DWORD WINAPI retrieverThreadProc(LPVOID parameter) {
    CPUInfo* info = (CPUInfo*)parameter;
    getCPUInfo(*info);
    return 0;
}


int getMultipleCPUInfo(CPUInfo* array) {
    DWORD processAffinityMask;
    DWORD systemAffinityMask;
    GetProcessAffinityMask(
        GetCurrentProcess(),
        &processAffinityMask,
        &systemAffinityMask);

    int totalQueried = 0;

    int processorCount = getCPUCount();
    HANDLE* handles = new HANDLE[processorCount];

    for (int i = 0; i < processorCount; ++i) {
        // Skip if processor is disabled.
        int currentMask = (1 << i);
        if (!(processAffinityMask & currentMask)) {
            continue;
        }

        HANDLE& handle = handles[totalQueried];

        DWORD dummy;
        handle = CreateThread(
            NULL, 0, retrieverThreadProc, array + totalQueried,
            CREATE_SUSPENDED, &dummy);
        if (!handle) {
            continue;
        }

        if (0 == SetThreadAffinityMask(handle, currentMask)) {
            CloseHandle(handle);
            continue;
        }

        ResumeThread(handle);
        ++totalQueried;
    }

    WaitForMultipleObjects(totalQueried, handles, TRUE, INFINITE);
    for (int i = 0; i < totalQueried; ++i) {
        CloseHandle(handles[i]);
    }
    delete[] handles;
    return totalQueried;
}

#else  // Linux

#include <sched.h>

int getCPUCount() {
    cpu_set_t mask;
    int res = sched_getaffinity(0, sizeof(mask), &mask);
    if (res == -1) {
        return 0;
    }

    int cpuCount = 0;
    for (int i = 0; i < CPU_SETSIZE; ++i) {
        if (CPU_ISSET(i, &mask)) {
            ++cpuCount;
        }
    }
    return cpuCount;
}


int getMultipleCPUInfo(CPUInfo* array) {
    cpu_set_t oldMask;
    int res = sched_getaffinity(0, sizeof(oldMask), &oldMask);
    if (res == -1) {
        return 0;
    }

    int cpuCount = getCPUCount();
    int totalQueried = 0;
    for (int i = 0; i < cpuCount; ++i) {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(i, &mask);
        res = sched_setaffinity(0, sizeof(mask), &mask);
        if (res == -1) {
            continue;
        }

        // Do we need to yield to make sure we're actually going to run on that
        // processor?  Not according to the online docs...  affinity changes
        // should happen immediately.
        // sched_yield();

        getCPUInfo(array[totalQueried]);
        ++totalQueried;
    }

    // Make sure we restore the original affinity.
    sched_setaffinity(0, sizeof(oldMask), &oldMask);

    return totalQueried;
}

#endif

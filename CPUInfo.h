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

#ifndef CPU_INFO_H
#define CPU_INFO_H


#include <string>


/**
 * Describes characteristics and features of an x86 processor.
 */
struct CPUInfo {
    /**
     * Returns a string representation of the manufacturer code, which is,
     * in turn, determined from the CPU's vendor id string.
     *
     * For example, on Intel processors:
     *   GenuineIntel -> CPUInfo::Intel -> "Intel Corporation"
     */
    const char* getVendorName() const;

    /**
     * Returns a string representation of the processor name.  This uses the
     * processor identification algorithm described in the Intel developer
     * manuals:  If an extended name is available, return it (after stripping
     * leading whitespace).  Then, try to use the brand index method.
     * (see getProcessorBrandName)  If that isn't supported, determine a name
     * from the family, model, and stepping values.
     * (see getClassicalProcessorName)
     */
    std::string getProcessorName() const;

    /**
     * Returns a string representation of the processor type code.
     * 0="Original OEM", 1="Intel OverDrive", 2="Dual Processor", etc.
     */
    const char* getProcessorTypeName() const;
    
    /**
     * Returns the brand name using the brand index method discussed in
     * getProcessorName.
     */
    std::string getProcessorBrandName() const;

    /**
     * Returns the classical name of the processor, using only the family,
     * model, and stepping values.
     */
    const char* getClassicalProcessorName() const;

    
    enum Manufacturer {
        AMD,
        Intel,
        NSC,
        UMC,
        Cyrix,
        NexGen,
        IDT,
        Rise,
        Transmeta,
        UnknownManufacturer
    };

    struct Identity {
        Manufacturer manufacturer;  ///< Guessed manufacturer based on vendor string.
        int type;                   ///< Processor type.  0=oem, 1=overdrive, etc.  Call getProcessorTypeName() for a string representation.
        int family;
        int model;
        int stepping;
        char vendor[12 + 1];        ///< GenuineIntel on Intel systems, etc.

        int brand;                  ///< Brand ID.  0 if not supported.

        // Extended identity.
        bool hasExtendedName;       ///< If false, the following fields are invalid.
        unsigned firstNonSpace;     ///< First non-whitespace character in extendedName.
        char extendedName[48 + 1];
    };

    struct Features {
        bool fpu;     ///< x87 FPU on Chip
        bool vme;     ///< Virtual-8086 Mode Enhancement
        bool de;      ///< Debugging Extensions
        bool pse;     ///< Page Size Extensions
        bool tsc;     ///< Time Stamp Counter
        bool msr;     ///< RDMSR and WRMSR Support
        bool pae;     ///< Physical Address Extensions
        bool mce;     ///< Machine Check Exception
        bool cx8;     ///< CMPXCHG8B Instruction
        bool apic;    ///< APIC on Chip
        bool sep;     ///< SYSENTER and SYSEXIT
        bool mtrr;    ///< Memory Type Range Registers
        bool pge;     ///< PTE Global Bit
        bool mca;     ///< Machine Check Architecture
        bool cmov;    ///< Conditional Move/Compare Instructions
        bool pat;     ///< Page Attribute Table
        bool pse36;   ///< Page Size Extension
        bool serial;  ///< Processor Serial Number
        bool clfsh;   ///< CLFLUSH Instruction
        bool ds;      ///< Debug Store
        bool acpi;    ///< Thermal Monitor and Clock Control
        bool mmx;     ///< MMX Technology
        bool fxsr;    ///< FXSAVE/FXRSTOR
        bool sse;     ///< SSE Extensions
        bool sse2;    ///< SSE2 Extensions
        bool ss;      ///< Self Snoop
        bool htt;     ///< Hyper-Threading Technology
        bool thermal; ///< Thermal Monitor
        bool ia64;    ///< IA64 Instructions
        bool pbe;     ///< Pending Break Enable

        /**
         * Whether floating-point SSE instructions work.  Not reported
         * from CPUID, but directly tested.
         */
        bool ssefp;

        char serialNumber[29 + 1];        // If serial=true.
        int logicalProcessorsPerPhysical; // If htt=true.
        int CLFLUSHCacheLineSize;         // If clfsh=true.
        int APIC_ID;                      // If apic=true.

        // Intel extended features.
        bool sse3;     ///< SSE3 Instructions
        bool monitor;  ///< MONITOR/MWAIT
        bool ds_cpl;   ///< CPL Qualified Debug Store
        bool est;      ///< Enhanced Intel SpeedStep Technology
        bool tm2;      ///< Thermal Monitor 2
        bool cnxt_id;  ///< L1 Context ID

        // AMD extended features.
        bool _3dnow;      ///< 3DNow! Instructions
        bool _3dnowPlus;  ///< 3DNow! Instructions Extensions
        bool ssemmx;      ///< SSE MMX
        bool mmxPlus;     ///< Same as SSEMMX on AMD, different bit on Cyrix.
        bool supportsMP;  ///< Used to differentiate between Athlon XP and MP.
    };

    struct Cache {
        int L1CacheSize;  // In KB.  Negative if not supported.
        int L2CacheSize;  // In KB.  Negative if not supported.
        int L3CacheSize;  // In KB.  Negative if not supported.
    };
    
    struct PowerManagement {
        bool ts;   ///< Temperature Sensor
        bool fid;  ///< Frequency ID
        bool vid;  ///< Voltage ID
        bool ttp;  ///< Thermal Trip
        bool tm;   ///< Thermal Monitoring
        bool stc;  ///< Software Thermal Control
    };


    /**
     * Set to true when the processor is detected to support the CPUID
     * instruction.  If false, nothing else in this struct is valid.
     */
    bool supportsCPUID;

    Identity        identity;         ///< Processor identity information.
    Features        features;         ///< Supported feature bits.
    Cache           cache;            ///< Information about on-chip cache.
    PowerManagement powerManagement;  ///< Advanced power management feature bits.

    /// Clock frequency in MHz.
    int frequency;
};


/**
 * Fills 'info' struct with information describing the current CPU.  On a
 * multiprocessing system, be careful to call this in a thread that runs on
 * only one CPU.
 */
void getCPUInfo(CPUInfo& info);


/**
 * Returns the number of CPUs in the system.
 */
int getCPUCount();


/**
 * Returns the info for all processors installed in the system.
 * 'array' must have at least getCPUCount() entries.  Returns the
 * actual number of processors successfully queried.
 */
int getMultipleCPUInfo(CPUInfo* array);


#endif

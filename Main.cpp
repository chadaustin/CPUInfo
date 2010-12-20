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

#include <stdio.h>
#include "CPUInfo.h"


void printCPUInfo(int processor, const CPUInfo& info) {
    printf("Processor %d:\n", processor);
    if (!info.supportsCPUID) {
        printf("  No CPUID Support\n");
        printf("\n");
        return;
    }

    printf("  Vendor:         %s\n", info.getVendorName());
    printf("  Name:           %s\n", info.getProcessorName().c_str());
    printf("  Type:           %s\n", info.getProcessorTypeName());
    printf("  Brand:          %s\n", info.getProcessorBrandName().c_str());
    printf("  Classical Name: %s\n", info.getClassicalProcessorName());
    printf("\n");
    printf("  Family:         %d\n", info.identity.family);
    printf("  Model:          %d\n", info.identity.model);
    printf("  Stepping:       %d\n", info.identity.stepping);
    printf("\n");
    printf("  Frequency:      %d MHz\n", info.frequency);
    printf("\n");
    printf("  Features:\n");
    
    bool featureflag = false;
#define F(flag, desc)                           \
    if (info.features.flag) {                   \
        featureflag = true;                     \
        printf("  %8s: %s\n", #flag, desc);     \
    }

    F(fpu,     "Floating Point Unit");
    F(vme,     "Virtual-8086 Mode Enhancement");
    F(de,      "Debugging Extensions");
    F(pse,     "Page Size Extensions");
    F(tsc,     "Time Stamp Counter");
    F(msr,     "RDMSR and WRMSR Support");
    F(pae,     "Physical Address Extensions");
    F(mce,     "Machine Check Exception");
    F(cx8,     "CMPXCHG8B Instruction");
    F(apic,    "APIC on Chip");
    F(sep,     "SYSENTER and SYSEXIT");
    F(mtrr,    "Memory Type Range Registers");
    F(pge,     "PTE Global Bit");
    F(mca,     "Machine Check Architecture");
    F(cmov,    "Conditional Move/Compare Instructions");
    F(pat,     "Page Attribute Table");
    F(pse36,   "Page Size Extension");
    F(serial,  "Serial Number Available");
    F(clfsh,   "CLFLUSH Instruction");
    F(ds,      "Debug Store");
    F(acpi,    "Thermal Monitor and Clock Control");
    F(mmx,     "MMX Technology");
    F(fxsr,    "FXSAVE/FXRSTOR Instructions");
    F(sse,     "SSE Extensions");
    F(ssefp,   "SSE Floating Point");
    F(sse2,    "SSE2 Extensions");
    F(ss,      "Self Snoop");
    F(htt,     "Hyper-Threading Technology");
    F(thermal, "Thermal Monitor");
    F(ia64,    "IA64 Instructions");
    F(pbe,     "Pending Break Enable");

    F(sse3,    "SSE3 Extensions");
    F(monitor, "MONITOR/MWAIT");
    F(ds_cpl,  "CPL Qualified Debug Store");
    F(est,     "Enhanced Intel SpeedStep Technology");
    F(tm2,     "Thermal Monitor 2");
    F(cnxt_id, "L1 Context ID");

    F(_3dnow,  "3DNow! Instructions");
    F(_3dnowPlus, "3DNow! Instructions Extensions");
    F(ssemmx,  "SSE MMX");
    F(mmxPlus, "MMX+");
    F(supportsMP, "Supports Multiprocessing");
    
#undef F

    if (!featureflag) {
        printf("    None\n");
    }

    printf("\n");

    if (info.features.serial) {
        printf("            Serial Number: %s\n", info.features.serialNumber);
    }
    if (info.features.htt) {
        printf("            Logical Processors per Physical: %d\n",
               info.features.logicalProcessorsPerPhysical);
    }
    if (info.features.clfsh) {
        printf("            CLFLUSH Cache Line Size: %d bytes\n",
               info.features.CLFLUSHCacheLineSize);
    }
    if (info.features.apic) {
        printf("            APIC ID: %d\n", info.features.APIC_ID);
    }

    printf("\n");

    printf("  Cache:\n");
    if (info.cache.L1CacheSize != -1) {
        printf("    L1 Size: %d kB\n", info.cache.L1CacheSize);
    }
    if (info.cache.L2CacheSize != -1) {
        printf("    L2 Size: %d kB\n", info.cache.L2CacheSize);
    }
    if (info.cache.L3CacheSize != -1) {
        printf("    L3 Size: %d kB\n", info.cache.L3CacheSize);
    }

    printf("\n");
    printf("  Enhanced Power Management:\n");

    bool pmflag = false;
#define PM(flag, desc)                          \
    if (info.powerManagement.flag) {            \
        pmflag = true;                          \
        printf("  %8s: %s\n", #flag, desc);     \
    }

    PM(ts,  "Temperature Sensor");
    PM(fid, "Frequency ID");
    PM(vid, "Voltage ID");
    PM(ttp, "Thermal Trip");
    PM(tm,  "Thermal Monitoring");
    PM(stc, "Software Thermal Control");

#undef PM

    if (!pmflag) {
        printf("    None\n");
    }

    printf("\n\n");
}


int main() {
    int processorCount = getCPUCount();
    CPUInfo* info = new CPUInfo[processorCount];
    int actual = getMultipleCPUInfo(info);
    for (int i = 0; i < actual; ++i) {
        printCPUInfo(i, info[i]);
    }

    delete[] info;
}

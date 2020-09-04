from __future__ import division
from collections import defaultdict

files = ["../mappings/postgres/M_L.txt", "../mappings/postgres/M_Lf.txt"]
fields = ["PID", "START", "END", "PRT", "RES", "PRES", "REF", "SHD", "FLAG", "TP", "PATH"]
library_start = int("0x800000000", 16)

def process_file(filename):
    print "Opening " + filename
    with open(filename) as f:
        main_count = defaultdict(lambda: 0)
        library_count = defaultdict(lambda: 0)
        for line in f:
            is_library = False
            line = line.strip()
            line = line.split()
            if line[3] == "r-x" or line[3] == "rwx":
                print line
                vaddr = start = int(line[fields.index("START")], 16)
                end = int(line[fields.index("END")], 16)
                if start >= library_start:
                    is_library = True
                sets = ""
                while vaddr < end:
                    tlb_set = vaddr_to_set(vaddr)
                    if is_library:
                        library_count[tlb_set] += 1
                    else:
                        main_count[tlb_set] += 1
                    sets += "\t" + str(tlb_set)
                    vaddr += 4096
                print sets
        main_total_set_usage = sum(main_count.values())
        library_total_set_usage = sum(library_count.values())
        total_set_usage = main_total_set_usage + library_total_set_usage
        print "Main executable usage of STLB"
        for tlb_set in range(128):
            print str(tlb_set) + "\t" + "{:.3%}".format(main_count[tlb_set] / main_total_set_usage)
        print "Library usage of STLB"
        for tlb_set in range(128):
            print str(tlb_set) + "\t" + "{:.3%}".format(library_count[tlb_set] / library_total_set_usage)
        print "Aggregate usage of STLB"
        for tlb_set in range(128):
            print str(tlb_set) + "\t" + "{:.3%}".format((main_count[tlb_set] + library_count[tlb_set]) / total_set_usage)

def vaddr_to_set(vaddr):
    """
    This encodes the indexing formula of the Skylake STLB, as reverse
    engineered by Weixi.
    """
    return ((vaddr >> 12) & 0x7f) ^ ((vaddr >> 19) & 0x7f)

for file in files:
    process_file(file)

from collections import defaultdict

pc_sample_file = "/Users/winnie/Documents/backup_root/research/devel/2019_ispass/results/java/final_results/ktrace/derby_clk_unhalted_addresses.txt"
vaddrs = {"user":
        {"start": int("0x400000", 16), "end": int("0xa76c00000", 16)},
        "jit":
        {"start": int("0x803a00000", 16), "end": int("0x804600000", 16)},
        "libjvm":
        {"start": int("0x801800000", 16), "end": int("0x802339000", 16)},}

def process_file(filename):
    print "Opening " + filename
    with open(filename) as f:
        total_user_samples = 0
        jit_samples = 0
        libjvm_samples = 0
        for line in f:
            line = line.strip()
            line = line.split()
            vaddr = int(line[0], 16)
            if vaddr >= vaddrs["user"]["start"] and vaddr < vaddrs["user"]["end"]:
                total_user_samples += 1
                if vaddr >= vaddrs["jit"]["start"] and vaddr < vaddrs["jit"]["end"]:
                    jit_samples += 1
                if vaddr >= vaddrs["libjvm"]["start"] and vaddr < vaddrs["libjvm"]["end"]:
                    libjvm_samples += 1

        print total_user_samples, "user samples", jit_samples, "JIT samples", libjvm_samples, "libjvm samples"

def vaddr_to_pindex(start, vaddr):
    return (vaddr - start) / 4096

process_file(pc_sample_file)

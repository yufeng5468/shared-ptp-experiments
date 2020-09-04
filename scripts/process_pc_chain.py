from collections import defaultdict

pc_sample_file = "/Users/winnie/Documents/backup_root/research/devel/2019_ispass/results/java/final_results/ktrace/javac_clk_unhalted_post_warmup_chain.txt"
# vaddrs = {"postgres":
#         {"start": int("0x400000", 16), "end": int("0x9f1000", 16)},
#         "libcrypto":
#         {"start": int("0x802a00000", 16), "end": int("0x802c43000", 16)},
#         "libc":
#         {"start": int("0x803400000", 16), "end": int("0x803594000", 16)},
#         "vdso":
#         {"start": int("0x7ffffffff000", 16), "end": int("0x800000000000", 16)}}
vaddrs = {"user":
        {"start": int("0x400000", 16), "end": int("0xa76c00000", 16)},
        "jit":
        {"start": int("0x803a00000", 16), "end": int("0x805000000", 16)},
        "libjvm":
        {"start": int("0x801800000", 16), "end": int("0x802339000", 16)},}
result = {}

def process_file_accessed_pages(filename):
    print "Opening " + filename
    with open(filename) as f:
        for key in vaddrs.keys():
            result[key] = set()
        is_top_of_stack = False
        for line in f:
            if line == "--\n":
                is_top_of_stack = True
                continue
            elif not is_top_of_stack:
                continue
            is_library = False
            line = line.strip()
            line = line.split()
            vaddr = int(line[0], 16)
            for key, value in vaddrs.items():
                if vaddr >= value["start"] and vaddr < value["end"]:
                    # print "adding " + str(vaddr) + " to " + key
                    result[key].add(vaddr_to_pindex(value["start"], vaddr))
            is_top_of_stack = False
        for key, value in result.items():
            print key + " " + str(len(value))
            print value

def process_file_sample_percentage(filename):
    print "Opening " + filename
    with open(filename) as f:
        for key in vaddrs.keys():
            result[key] = 0
        total_samples = 0
        is_top_of_stack = False
        for line in f:
            if line == "--\n":
                is_top_of_stack = True
                continue
            elif not is_top_of_stack:
                continue
            total_samples += 1
            line = line.strip()
            line = line.split()
            vaddr = int(line[0], 16)
            for key, value in vaddrs.items():
                if vaddr >= value["start"] and vaddr < value["end"]:
                    result[key] += 1
            is_top_of_stack = False
        for key, value in result.items():
            print key, value, "samples", float(value) / total_samples

def vaddr_to_pindex(start, vaddr):
    return (vaddr - start) / 4096

# process_file_accessed_pages(pc_sample_file)
process_file_sample_percentage(pc_sample_file)

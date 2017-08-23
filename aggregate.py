import sys, os
from collections import defaultdict
from itertools import count
from statistics import mean

result = defaultdict(list)

def parse_result(path):
    with open(path, "rt") as file:
        one_result = result[os.path.basename(path)]
        for i, line in zip(count(), file):
            if len(one_result) < i + 1:
                one_result.append(dict(sec=[], rss=[]))
            row = one_result[i]
            k, t, m = line.split("\t")
            row["sec"].append(float(t))
            row["rss"].append(float(m))

if __name__ == "__main__":
    base = sys.argv[1]
    for n in os.listdir(base):
        for file in os.listdir(os.path.join(base, n)):
            parse_result(os.path.join(base, n, file))
    for key, val in result.items():
        print(key)
        for i, row in zip(count(), val):
            print("{0}\t{1:.2f}s\t{2:.2f}M".format(i * 10, mean(row["sec"]), mean(row["rss"]) / 1024))

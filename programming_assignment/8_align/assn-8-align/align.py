#!/usr/bin/env python

import random  # for seed, random
import sys     # for stdout
import time

mem_dict = {}
def cache_index(func):
    def inner(*args, **kwargs):
        cache_key = (args[2], args[3])
        if cache_key in mem_dict:
            return mem_dict[cache_key]
        value = func(*args, **kwargs)
        mem_dict[cache_key] = value
        return value
    return inner


def findOptimalAlignment(strand1, strand2):
    return _findOptimalAlignment(strand1, strand2, 0, 0)


# Computes the score of the optimal alignment of two DNA strands.
@cache_index
def _findOptimalAlignment(strand1, strand2, index_1, index_2):
    # if one of the two strands is empty, then there is only
    # one possible alignment, and of course it's optimal
    if index_1 == len(strand1):
        return (len(strand2) * -2, ' ' * len(strand2), strand2)
    if index_2 == len(strand2):
        return (len(strand1) * -2, strand1, ' ' * len(strand1))

    # There's the scenario where the two leading bases of
    # each strand are forced to align, regardless of whether or not
    # they actually match.
    (bestWith, align1, align2) = findOptimalAlignment(strand1[1:], strand2[1:])
    if strand1[0] == strand2[0]:
        return (bestWith + 1, strand1[0] + align1, strand1[0] + align2)  # no benefit from making other recursive calls

    best = bestWith - 1
    align1 = strand1[0] + align1
    align2 = strand2[0] + align2

    # It's possible that the leading base of strand1 best
    # matches not the leading base of strand2, but the one after it.
    (bestWithout, align1Without, align2Without) = findOptimalAlignment(strand1, strand2[1:])
    bestWithout -= 2  # penalize for insertion of space
    if bestWithout > best:
        best = bestWithout
        align1 = ' ' + align1Without
        align2 = strand2[0] + align2Without

    # opposite scenario
    (bestWithout, align1Without, align2Without) = findOptimalAlignment(strand1[1:], strand2)
    bestWithout -= 2  # penalize for insertion of space
    if bestWithout > best:
        best = bestWithout
        align1 = strand1[0] + align1Without
        align2 = ' ' + align2Without

    return (best, align1, align2)


# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]
def generateRandomDNAStrand(minlength, maxlength):
    assert minlength > 0, \
           "Minimum length passed to generateRandomDNAStrand" \
           "must be a positive number"  # these \'s allow mult-line statements
    assert maxlength >= minlength, \
           "Maximum length passed to generateRandomDNAStrand must be at " \
           "as large as the specified minimum length"
    strand = ""
    length = random.choice(range(minlength, maxlength + 1))
    bases = ['A', 'T', 'G', 'C']
    for i in range(0, length):
        strand += random.choice(bases)
    return strand


# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.
def printAlignment(score, align1, align2, seconds_elapsed, out=sys.stdout):
    out.write("Optimal alignment score is " + str(score) + "\n")
    out.write("                           " + align1 + "\n")
    out.write("                           " + align2 + "\n")
    out.write(f"Took {seconds_elapsed} seconds.\n")

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.


def main():
    while True:
        sys.stdout.write("Generate random DNA strands?\n")
        answer = sys.stdin.readline()
        if answer == "no\n":
            break
        strand1 = generateRandomDNAStrand(12, 12)
        strand2 = generateRandomDNAStrand(12, 12)
        sys.stdout.write("Aligning these two strands: " + strand1 + "\n")
        sys.stdout.write("                            " + strand2 + "\n")
        start_time = time.perf_counter()
        best, align1, align2 = findOptimalAlignment(strand1, strand2)
        seconds_elapsed = time.perf_counter() - start_time
        printAlignment(best, align1, align2, seconds_elapsed)


if __name__ == "__main__":
    main()

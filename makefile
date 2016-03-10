PROGRAMS=create_bin create_bin2_multiplex_kmers create_bin2_aleatmult_kmers create_bin2_simple_kmers naive_method read_bin sort_bin merge_bin merge_bin_tree merge_bin_parallel count_overlaps count_overlaps2 search_kmer_overlaps search_simple_kmer_overlaps get_stats_overlaps
TESTS=

CPPFLAGS=-g -O0 -Wall -Werror -std=c99
CFLAGS=
LDFLAGS=
LDLIBS= -lm

all: $(PROGRAMS) $(TESTS) 

create_bin: create_bin.o
create_bin2_multiplex_kmers: create_bin2_multiplex_kmers.o
create_bin2_aleatmult_kmers: create_bin2_aleatmult_kmers.o
create_bin2_simple_kmers: create_bin2_simple_kmers.o
naive_method: naive_method.o
read_bin: read_bin.o
sort_bin: sort_bin.o
merge_bin: merge_bin.o
merge_bin_tree: merge_bin_tree.o
merge_bin_parallel: merge_bin_parallel.o
count_overlaps: count_overlaps.o
count_overlaps2: count_overlaps2.o
search_kmer_overlaps: search_kmer_overlaps.o
search_simple_kmer_overlaps: search_simple_kmer_overlaps.o
get_stats_overlaps: get_stats_overlaps.o

libcountoverlaps.a: libcountoverlaps()

clean:
	-rm -rf *.o
	-rm -rf *.a
	-rm -rf $(TESTS)
	-rm -rf $(PROGRAMS)

.PHONY: all clean

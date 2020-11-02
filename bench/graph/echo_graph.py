#!/usr/bin/env python3

import sys
import subprocess

from bench_graph import (
    Benchmark,
    make_fullnames,
    read_all,
    plot_trials_for_types,
    plot_cdf_for_types,
)


def main():
    """main function
    """

    build_dir = sys.argv[1]
    repetitions = 100
    if len(sys.argv) >= 3:
        repetitions = sys.argv[2]

    prefix = build_dir + '/bench/'

    subprocess.run(['../bin/mprpc_bench_graph_echo', '-r', str(repetitions)],
                   cwd=prefix, check=True)

    test_name = 'echo'
    test_types = ['tcp', 'udp']
    data_sizes = [1, 32, 1024, 32 * 1024, 1024 * 1024]

    fullnames = make_fullnames(test_name, test_types, data_sizes)
    results = read_all(fullnames, prefix)

    bench = Benchmark(test_name, test_types, data_sizes, results)

    plot_trials_for_types(bench, 1, 2, prefix + 'echo_trials_types.png')
    plot_cdf_for_types(bench, 2, 1, prefix + 'echo_cdf_types.png')


if __name__ == '__main__':
    main()

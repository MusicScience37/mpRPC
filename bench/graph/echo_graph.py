#!/usr/bin/env python3

import argparse
import sys
import subprocess
import time

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

    parser = argparse.ArgumentParser(description='Test of echo')
    parser.add_argument('--build', '-b', dest='build', required=True)
    parser.add_argument('--host', '-H', dest='host',
                        action='store', default='127.0.0.1', type=str)
    parser.add_argument('--repetitions', '-r', dest='repetitions',
                        action='store', default=100, type=int)
    parser.add_argument('--no_server', '-n', dest='use_server',
                        action='store_false', default=True)
    args = parser.parse_args()

    build_dir = args.build
    repetitions = args.repetitions
    host = args.host
    use_server = args.use_server

    prefix = build_dir + '/bench/'

    server = None
    if use_server:
        server = subprocess.Popen(
            ['../bin/mprpc_bench_graph_echo_server', '--host', host], cwd=prefix)
        time.sleep(0.2)

    subprocess.run(['../bin/mprpc_bench_graph_echo_client', '--host', host, '-r', str(repetitions)],
                   cwd=prefix, check=True)

    if server:
        server.terminate()
        server.wait()

    test_name = 'echo'
    test_types = ['tcp', 'tcp_zstd', 'udp', 'udp_zstd']
    data_sizes = [1, 32, 1024, 32 * 1024, 1024 * 1024]

    fullnames = make_fullnames(test_name, test_types, data_sizes)
    results = read_all(fullnames, prefix)

    bench = Benchmark(test_name, test_types, data_sizes, results)

    plot_trials_for_types(bench, 1, len(test_types),
                          prefix + 'echo_trials_types.png')
    plot_cdf_for_types(bench, len(test_types), 1,
                       prefix + 'echo_cdf_types.png')


if __name__ == '__main__':
    main()

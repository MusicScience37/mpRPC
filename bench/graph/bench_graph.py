"""common functions for graphs of benchmarks
"""

import os
from typing import Dict, List
import numpy as np
import matplotlib.pyplot as plt
import logging
from collections import namedtuple


Benchmark = namedtuple('Benchmark', [
    'test_name',
    'test_types',
    'data_sizes',
    'results',
])


logging.basicConfig(level=logging.INFO)


def make_fullnames(test_name: str, test_types: List[str], data_sizes: List[int]):
    """make fullnames of benchmarks
    """

    return sum(
        [
            [
                test_name + '_' + test_type + '_' + str(data_size)
                for data_size in data_sizes
            ]
            for test_type in test_types
        ],
        []
    )


def read_vector(filepath: str) -> np.ndarray:
    """read a vector from a file
    """

    logging.info('loading %s', filepath)
    with open(filepath) as file:
        return np.array([float(line) for line in file])


def read_all(filenames: List[str], prefix: str) -> Dict[str, np.ndarray]:
    """read all files
    """

    filenames = list(
        filter(lambda filename: os.path.isfile(prefix + filename + '.txt'), filenames))
    return {
        filename: read_vector(prefix + filename + '.txt')
        for filename in filenames
    }


def plot_trials_to_axis(ax, filepaths: List[str], results: Dict[str, np.ndarray], labels: List[str] = None):
    """plot trials in results to an axis
    """

    if labels is None:
        labels = filepaths

    for ind in range(len(filepaths)):
        filepath = filepaths[ind]
        label = labels[ind]

        if not filepath in results.keys():
            continue

        y = results[filepath]
        size = y.size
        x = np.linspace(1, size, size)
        ax.plot(x, 1e-6 * y, label=label)

    ax.set_yscale('log')

    max_size = max([time.size for time in results.values()])
    ax.set_xlim(0, max_size)

    ax.xaxis.grid(True)
    ax.yaxis.grid(True)

    ax.legend()


def plot_trials_for_types(bench: Benchmark, rows: int, cols: int, output: str):
    """plot trials in results for each type
    """

    logging.info('plotting trials in results for each type')

    positions = sum(
        [
            [(row, col) for col in range(cols)]
            for row in range(rows)
        ],
        []
    )
    fig, axes = plt.subplots(nrows=rows, ncols=cols,
                             figsize=[max(cols * 2, 6.4),
                                      max(rows * 2 + 1, 4.8)],
                             sharex=True, sharey=True, squeeze=False)

    fig.suptitle('Time of RPC Calls ({})'.format(bench.test_name))

    labels = ['{} bytes'.format(size) for size in bench.data_sizes]

    for type_ind in range(len(bench.test_types)):
        test_type = bench.test_types[type_ind]
        ax = axes[positions[type_ind][0], positions[type_ind][1]]

        ax.set_title(test_type)

        if positions[type_ind][0] == rows - 1:
            ax.set_xlabel('Trial Number')
        if positions[type_ind][1] == 0:
            ax.set_ylabel('Time [ms]')

        fullnames = make_fullnames(
            bench.test_name, [test_type], bench.data_sizes)
        plot_trials_to_axis(ax, fullnames, bench.results, labels)

    fig.savefig(output)


def plot_cdf_to_axis(ax, filepaths: List[str], results: Dict[str, np.ndarray], labels: List[str] = None):
    """plot cumulative distribution function (CDF) of the results to an axis
    """

    if labels is None:
        labels = filepaths

    for filepath in filepaths:
        if not filepath in results.keys():
            continue

        x = results[filepath].copy()
        x.sort()
        size = x.size
        y = np.linspace(1 / size, 1, size)
        ax.plot(1e-6 * x, y)

    ax.set_xscale('log')

    ax.set_ylim(0, 1)

    ax.xaxis.grid(True)
    ax.yaxis.grid(True)

    ax.legend(labels)


def plot_cdf_for_types(bench: Benchmark, rows: int, cols: int, output: str):
    """plot cumulative distribution function (CDF) of the results for each type
    """

    logging.info('plotting CDF of results for each type')

    positions = sum(
        [
            [(row, col) for col in range(cols)]
            for row in range(rows)
        ],
        []
    )
    fig, axes = plt.subplots(nrows=rows, ncols=cols,
                             figsize=[max(cols * 2, 6.4),
                                      max(rows * 2 + 1, 4.8)],
                             sharex=True, sharey=True, squeeze=False)

    fig.suptitle('CDF of Time of RPC Calls ({})'.format(bench.test_name))

    labels = ['{} bytes'.format(size) for size in bench.data_sizes]

    for type_ind in range(len(bench.test_types)):
        test_type = bench.test_types[type_ind]
        ax = axes[positions[type_ind][0], positions[type_ind][1]]

        ax.set_title(test_type)

        if positions[type_ind][0] == rows - 1:
            ax.set_xlabel('Time [ms]')
        if positions[type_ind][1] == 0:
            ax.set_ylabel('Probability')

        fullnames = make_fullnames(
            bench.test_name, [test_type], bench.data_sizes)
        plot_cdf_to_axis(ax, fullnames, bench.results, labels)

    fig.savefig(output)

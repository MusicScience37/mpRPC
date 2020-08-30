"""test of writing logs with write_log.cpp
"""
import re
import subprocess


def call_writer(writer: str, args: list) -> list:
    """call writer to get list of logs
    """

    result = subprocess.run(
        [writer] + args, stdout=subprocess.PIPE, check=True, encoding='utf8')
    return result.stdout.splitlines()


def test_write_log_default(writer: str):
    """test of write_log with default configuration
    """

    log_lines = call_writer(writer, [])
    log_iterator = log_lines.__iter__()

    time_regex_str = R'\[\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d.\d\d\d\d\d\d\]'

    line = log_iterator.__next__()
    assert re.match(
        time_regex_str + R' \[info\]  \(thread \d*\) test info \(test\.cpp:3\)', line)

    line = log_iterator.__next__()
    assert re.match(
        time_regex_str + R' \[warn\]  \(thread \d*\) test warn \(test\.cpp:4\)', line)

    line = log_iterator.__next__()
    assert re.match(
        time_regex_str + R' \[error\] \(thread \d*\) test error \(test\.cpp:5\)', line)

    line = log_iterator.__next__()
    assert re.match(
        time_regex_str + R' \[fatal\] \(thread \d*\) test fatal \(test\.cpp:6\)', line)

    line = log_iterator.__next__()
    assert re.match(
        time_regex_str + R' \[info\]  \(thread \d*\) value: 37, 1.5 \(test\.cpp:10\)', line)

    line = log_iterator.__next__()
    assert re.match(
        time_regex_str + R' \[warn\]  \(thread \d*\) ' +
        R'error 37 test error with data: ' +
        R'\["abc",2.50000F,hex\(A9F13C\)\] '
        R'\(test\.cpp:20\)',
        line)

    for _ in range(3):
        line = log_iterator.__next__()
        assert re.match(
            time_regex_str + R' \[info\]  \(thread \d*\) test logger in a thread \d \(test\.cpp:30\)', line)


def test_write_log_verbose(writer: str):
    """test of write_log with verbose configuration
    """

    log_lines = call_writer(writer, ['-v'])
    log_iterator = log_lines.__iter__()

    time_regex_str = R'\[\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d.\d\d\d\d\d\d\]'

    line = log_iterator.__next__()
    assert re.match(
        time_regex_str + R' \[trace\] \(thread \d*\) test trace \(test\.cpp:1\)', line)

    line = log_iterator.__next__()
    assert re.match(
        time_regex_str + R' \[debug\] \(thread \d*\) test debug \(test\.cpp:2\)', line)

    line = log_iterator.__next__()
    assert re.match(
        time_regex_str + R' \[info\]  \(thread \d*\) test info \(test\.cpp:3\)', line)

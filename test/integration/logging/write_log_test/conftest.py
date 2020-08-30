"""configuration of pytest
"""
import pytest


def pytest_addoption(parser):
    """add option for pytest
    """

    parser.addoption("--writer", action="store",
                     default="write_log", help="path to write_log executable")


@pytest.fixture
def writer(request):
    return request.config.getoption("--writer")

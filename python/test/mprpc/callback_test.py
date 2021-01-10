"""test of callback function
"""

from mprpc_cpp_test import call_int_adder


def test_callback():
    """test of callback function
    """

    assert call_int_adder(lambda x, y: x + y, 2, 3) == 5

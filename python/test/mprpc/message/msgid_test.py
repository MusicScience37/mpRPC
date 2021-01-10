"""test of functions related to message ID
"""

from mprpc.message import is_valid_msgid


def test_is_valid_msgid():
    """test of is_valid_msgid function
    """

    assert is_valid_msgid(0)
    assert not is_valid_msgid(-1)
    assert is_valid_msgid(2 ** 32 - 1)
    assert not is_valid_msgid(2 ** 32)

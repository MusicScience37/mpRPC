"""test of ErrorCode enumeration
"""

from mprpc import ErrorCode


def test_error_code():
    """test of ErrorCode enumeration
    """

    assert ErrorCode.SUCCESS == 0

    names = [name for name in ErrorCode.__dict__ if name[0] != '_']
    properties = [getattr(ErrorCode, name) for name in names]
    values = [int(value) for value in properties]
    for value in properties:
        assert values.count(int(value)) == 1
        assert int(value) >= 0

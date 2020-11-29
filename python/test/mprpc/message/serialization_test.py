"""test of functions to serialize / deserialize data
"""

import pytest

from mprpc.message import pack_object, unpack_object, MessageData
from mprpc import MPRPCException, ErrorInfo, ErrorCode


def test_unpack_object():
    """test of unpack_object function
    """

    data = MessageData(b'\x93\x01\x02\x03')
    obj = unpack_object(data)
    assert isinstance(obj, list)
    assert obj == [1, 2, 3]

    data = MessageData(b'\x93\x01\x02')
    with pytest.raises(MPRPCException) as err:
        unpack_object(data)
    assert isinstance(err.value.args[0], ErrorInfo)
    assert err.value.args[0].code == int(ErrorCode.PARSE_ERROR)


def test_pack_object():
    """test of pack_object function
    """

    origin = [1, 'test']
    data = pack_object(origin)
    assert isinstance(data, MessageData)
    assert unpack_object(data) == origin

    origin = int
    with pytest.raises(MPRPCException) as err:
        pack_object(origin)
    assert isinstance(err.value.args[0], ErrorInfo)
    assert err.value.args[0].code == int(ErrorCode.UNEXPECTED_ERROR)

"""test of functions to serialize / deserialize data
"""

import pytest

from mprpc.message import (
    pack_object, unpack_object,
    pack_request, pack_response, pack_notification,
    MessageData,
    validate_message,
    Request, Response, Notification,
    MsgType,
)
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


def test_pack_request():
    """test of pack_request function
    """

    msgid = 37
    method = 'abc'
    params = [1, 'param']

    data = pack_request(msgid=msgid, method=method, params=params)
    req = validate_message(unpack_object(data))
    assert req == Request(MsgType.REQUEST, msgid, method, params)


def test_pack_response():
    """test of pack_response function
    """

    msgid = 37

    data = pack_response(msgid=msgid)
    res = validate_message(unpack_object(data))
    assert res == Response(MsgType.RESPONSE, msgid, None, None)

    result = 'abc'

    data = pack_response(msgid=msgid, result=result)
    res = validate_message(unpack_object(data))
    assert res == Response(MsgType.RESPONSE, msgid, None, result)

    error = 123

    data = pack_response(msgid=msgid, error=error)
    res = validate_message(unpack_object(data))
    assert res == Response(MsgType.RESPONSE, msgid, error, None)


def test_pack_notification():
    """test of pack_notification function
    """

    method = 'abc'
    params = [1, 'param']

    data = pack_notification(method=method, params=params)
    notification = validate_message(unpack_object(data))
    assert notification == Notification(MsgType.NOTIFICATION, method, params)

"""test of functions to parse messages
"""

import pytest
from typing import Any

from mprpc.message import (
    Request, Response, Notification, parse_message, MsgType
)
from mprpc import MPRPCException, ErrorInfo, ErrorCode


def _check_parse_error(data: Any):
    """check that parse error is raised when parsing data

    Parameters
    ----------
    data : Any
        data
    """

    with pytest.raises(MPRPCException) as err:
        parse_message(data)
    assert isinstance(err.value.args[0], ErrorInfo)
    assert err.value.args[0].code == int(ErrorCode.PARSE_ERROR)


def test_parse_request():
    """test of parsing request messages
    """

    msgtype = 0
    msgid = 37
    method = 'abc'
    params = [1, 'test', 3.14]
    data = [msgtype, msgid, method, params]

    req = parse_message(data)
    assert isinstance(req, Request)
    assert req.msgtype == MsgType.REQUEST
    assert req.msgid == msgid
    assert req.method == method
    assert req.params == params

    data = [msgtype, msgid, method]
    _check_parse_error(data)

    data = [msgtype, msgid, method, params, None]
    _check_parse_error(data)

    data = [msgtype, '37', method, params]
    _check_parse_error(data)

    data = [msgtype, -1, method, params]
    _check_parse_error(data)

    data = [msgtype, msgid, b'abc', params]
    _check_parse_error(data)

    data = [msgtype, msgid, method, 5]
    _check_parse_error(data)


def test_parse_response():
    """test of parsing response messages
    """

    msgtype = 1
    msgid = 37
    error = None
    result = 'result text'
    data = [msgtype, msgid, error, result]

    res = parse_message(data)
    assert isinstance(res, Response)
    assert res.msgtype == int(MsgType.RESPONSE)
    assert res.msgid == msgid
    assert res.error == error
    assert res.result == result

    data = [msgtype, msgid, error]
    _check_parse_error(data)

    data = [msgtype, msgid, error, result, None]
    _check_parse_error(data)

    data = [msgtype, '37', error, result]
    _check_parse_error(data)

    data = [msgtype, -1, error, result]
    _check_parse_error(data)


def test_parse_notification():
    """test of parsing notification messages
    """

    msgtype = 2
    method = 'abc'
    params = [1, 'test', 3.14]
    data = [msgtype, method, params]

    notification = parse_message(data)
    assert isinstance(notification, Notification)
    assert notification.msgtype == MsgType.NOTIFICATION
    assert notification.method == method
    assert notification.params == params

    data = [msgtype, method]
    _check_parse_error(data)

    data = [msgtype, method, params, None]
    _check_parse_error(data)

    data = [msgtype, b'abc', params]
    _check_parse_error(data)

    data = [msgtype, method, {'abc': 0}]
    _check_parse_error(data)


def test_parse_message():
    """test of parsing other messages
    """

    data = None
    _check_parse_error(data)

    data = {'abc': None}
    _check_parse_error(data)

    data = []
    _check_parse_error(data)

    data = ['2', 'method', ['param']]
    _check_parse_error(data)

    data = [-1, 0, 'test', [1, 2, 3]]
    _check_parse_error(data)

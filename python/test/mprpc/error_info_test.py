"""test of ErrorInfo class
"""

# This test actually uses mprpc module in eval function
# pylint: disable=unused-import
import mprpc
from mprpc import ErrorInfo
from mprpc.message import MessageData


def test_error_info():
    """test of ErrorInfo class 
    """

    err = ErrorInfo()
    assert err.code == 0
    assert err.message == ""
    assert err.data == MessageData()
    assert not err

    expected_str = "mprpc.ErrorInfo()"
    assert str(err) == expected_str
    # pylint: disable=eval-used
    assert str(eval(repr(err))) == expected_str

    code = 37
    message = 'test message'
    err = ErrorInfo(code, message)
    assert err.code == code
    assert err.message == message
    assert err.data == MessageData()
    assert err

    expected_str = "mprpc.ErrorInfo(code=37, message='test message')"
    assert str(err) == expected_str
    # pylint: disable=eval-used
    assert str(eval(repr(err))) == expected_str

    data = MessageData(b'\x93\x01\x02\x03')
    err = ErrorInfo(code, message, data)
    assert err.code == code
    assert err.message == message
    assert err.data == data
    assert err

    expected_str = "mprpc.ErrorInfo(code=37, message='test message', " + \
        "data=<mprpc.message.MessageData: [1,2,3]>)"
    assert str(err) == expected_str
    # pylint: disable=eval-used
    assert str(eval(repr(err))) == expected_str

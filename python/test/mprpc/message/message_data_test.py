"""test of MessageData class
"""

# This test actually uses mprpc module in eval function
# pylint: disable=unused-import
import mprpc
from mprpc.message import MessageData


def test_message_data():
    """test of MessageData class
    """

    origin = b'\x93\x01\x02\x03'
    data = MessageData(origin)

    assert data.data() == origin
    assert bytes(data) == origin

    assert str(data) == '<mprpc.message.MessageData: [1,2,3]>'

    # pylint: disable=eval-used
    assert eval(repr(data)).data() == origin

    assert MessageData(origin) == MessageData(origin)
    assert MessageData(origin) != MessageData(b'\x93\x01\x02\x04')

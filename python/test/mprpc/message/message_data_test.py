"""test of MessageData class
"""

from mprpc.message import MessageData


def test_message_data():
    """test of MessageData class
    """

    origin = b'\x01\x02\x03'
    data = MessageData(origin)
    assert data.data() == origin

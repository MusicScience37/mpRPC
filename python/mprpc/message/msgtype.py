"""message definition
"""

from enum import IntEnum


class MsgType(IntEnum):
    """enumeration of message types

    Attributes
    ----------
    REQUEST : int
        request (0)
    RESPONSE : int
        response (1)
    NOTIFICATION : int
        notification (2)
    """

    REQUEST = 0  # : request
    RESPONSE = 1  # : response
    NOTIFICATION = 2  # : notification

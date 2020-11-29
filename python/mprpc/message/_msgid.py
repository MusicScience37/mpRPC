"""implementation of functions and definitions related to message ID
"""

# maximum value for message ID
MAX_MSGID = pow(2, 32) - 1


def is_valid_msgid(msgid: int) -> bool:
    """check whether a message ID value is valid

    Parameters
    ----------
    msgid : int
        messsage ID, assumed to be an integer value.

    Returns
    -------
    bool
        whether `msgid` is valid
    """

    return 0 <= msgid <= MAX_MSGID

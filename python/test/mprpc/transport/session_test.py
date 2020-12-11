"""test of Session class
"""

from mprpc_cpp_test import MockSession


def test_session():
    """test of Session class
    """

    session = MockSession()

    assert session.remote_address.full_address == 'mock'
    assert str(session) == '<mprpc.transport.Session: remote_address=mock>'
    assert repr(session) == '<mprpc.transport.Session: remote_address=mock>'

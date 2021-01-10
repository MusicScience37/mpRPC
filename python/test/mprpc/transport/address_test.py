"""test of Address class
"""

from mprpc_cpp_test import MockAddress


def test_address():
    """test of Address class
    """

    address = MockAddress()
    assert address.full_address == 'mock'
    assert str(address) == '<mprpc.transport.Address: full_address=mock>'
    assert repr(address) == '<mprpc.transport.Address: full_address=mock>'

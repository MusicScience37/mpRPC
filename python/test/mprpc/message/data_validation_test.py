"""test of functions to validate data
"""

import pytest

from example_schemas import User, UserSchema, VectorSchema

from mprpc.message import validate_data, validate_data_list
from mprpc import MPRPCException, ErrorInfo, ErrorCode


def test_validate_data():
    """test of validate_data function
    """

    name = 'Test Name'
    age = 37
    data = {'name': name, 'age': age}

    user = validate_data(data)
    assert isinstance(user, dict)
    assert user == data

    schema = UserSchema()

    user = validate_data(data, schema)
    assert isinstance(user, User)
    assert user == User(**data)

    data = {'name': name}
    with pytest.raises(MPRPCException) as err:
        validate_data(data, schema)
    assert isinstance(err.value.args[0], ErrorInfo)
    assert err.value.args[0].code == int(ErrorCode.PARSE_ERROR)


def test_validate_data_list():
    """test of validate_data_list function
    """

    name = 'Test Name'
    age = 37
    vector = [1, 2, 3]
    data = [{'name': name, 'age': age}, {'vector': vector}]

    params = validate_data_list(data)
    assert isinstance(params[0], dict)
    assert params == data

    user_schemas = UserSchema()
    vector_schemas = VectorSchema()
    schemas = [user_schemas, vector_schemas]

    params = validate_data_list(data, schemas)
    assert isinstance(params[0], User)
    assert params == [User(name, age), {'vector': vector}]

    data = {'abc': 'def'}
    with pytest.raises(MPRPCException) as err:
        validate_data_list(data, schemas)
    assert isinstance(err.value.args[0], ErrorInfo)
    assert err.value.args[0].code == int(ErrorCode.PARSE_ERROR)

    data = [{'name': name, 'age': age}]
    with pytest.raises(MPRPCException) as err:
        validate_data_list(data, schemas)
    assert isinstance(err.value.args[0], ErrorInfo)
    assert err.value.args[0].code == int(ErrorCode.PARSE_ERROR)

    data = [{'name': name, 'age': age}, {'vector': vector}, {'vector': vector}]
    with pytest.raises(MPRPCException) as err:
        validate_data_list(data, schemas)
    assert isinstance(err.value.args[0], ErrorInfo)
    assert err.value.args[0].code == int(ErrorCode.PARSE_ERROR)

    data = [{'name': name, 'age': age}, {'vector': 0}]
    with pytest.raises(MPRPCException) as err:
        validate_data_list(data, schemas)
    assert isinstance(err.value.args[0], ErrorInfo)
    assert err.value.args[0].code == int(ErrorCode.PARSE_ERROR)

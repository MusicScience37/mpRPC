"""implementation of functions to validate data
"""

from typing import Any, Optional, List
import marshmallow

from mprpc import MPRPCException, ErrorInfo, ErrorCode


def validate_data(data: Any, schema: Optional[marshmallow.Schema] = None) -> Any:
    """validate data

    This function validate `data` with `schema` using `schema.load` function,
    and returns the result.

    Parameters
    ----------
    data : Any
        data to validate
    schema : Optional[Schema], optional
        schema of data, by default None to specify no validation.
        If an object other than None is provided,
        it is assumed to be an object of a class inherited from marshmallow.Schema.

    Returns
    -------
    Any
        validated data,
        `data` itself or an object of user-defined types
        (when `@post_load` decorator is used).

    Raises
    ------
    mprpc.MPRPCException
        If data is invalid.
    """

    if schema is None:
        # no validation is needed
        return data

    try:
        return schema.load(data)
    except marshmallow.ValidationError as err:
        raise MPRPCException(
            ErrorInfo(ErrorCode.PARSE_ERROR, 'failed to parse data: %s' % data)) from err


def validate_data_list(data: Any,
                       schemas: Optional[List[Optional[marshmallow.Schema]]] = None) -> List[Any]:
    """validate a list of data

    This function repeately calls :py:func:`mprpc.message.validate_data`
    to validate a list of data,
    used for validation of parameters in request and notification messages.

    Parameters
    ----------
    data : Any
        list of data to validate
    schemas : Optional[List[Optional[marshmallow.Schema]]], optional
        list of schemas, by default None to specify no validation.
        If a list is given, each element in the list is assumed to be None (no validation) or
        an object of a class inherited from marshmallow.Schema to use for validation.

    Returns
    -------
    List[Any]
        validated data

    Raises
    ------
    mprpc.MPRPCException
        If data is invalid.
    """

    if schemas is None:
        # no validation is needed
        return data

    if not isinstance(data, list):
        raise MPRPCException(
            ErrorInfo(ErrorCode.PARSE_ERROR, 'data is not a list: %s' % data))

    if len(schemas) != len(data):
        raise MPRPCException(
            ErrorInfo(ErrorCode.PARSE_ERROR, 'invalid length of data: %s' % data))

    return [
        validate_data(data_elem, schema)
        for (data_elem, schema) in zip(data, schemas)
    ]

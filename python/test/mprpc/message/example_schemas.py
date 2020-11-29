"""data schemas for test
"""

from typing import NamedTuple
from marshmallow import Schema, fields, validate, post_load


class User(NamedTuple):
    """class for test
    """

    name: str
    age: int


class UserSchema(Schema):
    """schema class for test
    """

    name = fields.String(required=True)
    age = fields.Integer(required=True, validate=validate.Range(min=0))

    @post_load
    def create_user(self, data, **_):  # pylint: disable=no-self-use
        """create User object
        """
        return User(**data)


class VectorSchema(Schema):
    """schema class for test
    """

    vector = fields.List(fields.Integer())

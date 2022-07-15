"""
Class for JSON Serialization
"""

from json import JSONEncoder

class MyEncoder(JSONEncoder):
    """
    Simple JSON encoder using the __dict__ method
    """

    def default(self, o):
        if isinstance(o, set):
            return list(o)
        else :
            return o.__dict__

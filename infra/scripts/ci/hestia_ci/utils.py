import argparse
from typing import NamedTuple, Sequence, Self
from dataclasses import fields, dataclass, MISSING

@dataclass
class ParsableArgs:
    """
    Dataclass-based base class with automatic field parsing via argparse
    """
    @classmethod
    def add_arguments(cls, parser: argparse.ArgumentParser) -> None:
        group = parser.add_argument_group(f"Arguments for {cls.__name__}")
        for f in fields(cls):
            if f.default == MISSING:
                group.add_argument(f"--{f.name}", type=f.type, required=True, help=f.metadata.get('help'))
            else: 
                group.add_argument(f"--{f.name}", type=f.type, default=f.default, help=f.metadata.get('help'))

class Version(NamedTuple):
    major: int = 0
    minor: int = 0
    patch: int = 0

    @classmethod
    def from_string(cls, version_str: str):
        return cls(*[int(x) for x in version_str.split(".", 3)])

    def incr_major(self):
        return Version(major=self.major+1)

    def incr_minor(self):
        return Version(major = self.major, minor=self.minor+1)

    def incr_patch(self):
        return self + (0,0,1)

    def __add__(self, other):
        return Version(*map(sum, zip(self, other)))

    def __str__(self) -> str:
        return '.'.join(map(str, self))

from typing import NamedTuple


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

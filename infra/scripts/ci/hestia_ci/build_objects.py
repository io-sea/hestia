import string
import json
from pathlib import Path
from dataclasses import dataclass

from hestia_ci.utils import ParsableArgs

@dataclass
class BuildInfo(ParsableArgs):    
    project_name: str
    version: str
    arch: str

@dataclass
class BuildArtifact:
    name: str
    type: str
    description: str
    optional: bool = False
    link_type: str = "other"

    @classmethod
    def from_dict(cls, artifact_dict: dict):
        return cls(**artifact_dict)
        
    def get_path(self) -> Path:
        return Path(self.name) / "." / self.type


class BuildArtifacts:

    def __init__(self, build_info: BuildInfo) -> None:
        self.artifacts = []
        self.build_info = build_info

    def load(self, artifact_file: Path) -> None:
        with open(artifact_file, 'r') as f:
            artifact_file_content = string.Template(f.read())

        artifact_file_content.substitute({
            "PROJECT": self.build_info.project_name,
            "VERSION": self.build_info.version,
            "ARCH": self.build_info.arch
        })

        artifact_json = json.loads(str(artifact_file_content))
        self.artifacts = [BuildArtifact.from_dict(a) for a in artifact_json]

import string
import json
from pathlib import Path


class BuildInfo():

    def __init__(self, project_name: str, version: str, arch: str) -> None:
        self.project_name = project_name
        self.version = version
        self.arch = arch


class BuildArtifact():

    def __init__(self, artifact_dict: dict) -> None:
        self.name = artifact_dict["name"]
        self.type = artifact_dict["type"]
        self.link_type = artifact_dict["link_type"]
        self.description = artifact_dict["description"]
        self.optional = False
        if "optional" in artifact_dict:
            self.optional = artifact_dict["optional"]

    def get_path(self) -> Path:
        return self.name / "." / self.type


class BuildArtifacts():

    def __init__(self, build_info: BuildInfo) -> None:
        self.artifacts = []
        self.build_info = build_info

    def load(self, artifact_file: Path):
        with open(artifact_file, 'r') as f:
            artifact_file_content = string.Template(f.read())

        artifact_file_content.substitute({
            "PROJECT", self.build_info.project_name,
            "VERSION", self.build_info.version,
            "ARCH", self.build_info.arch,
        })

        artifact_json = json.loads(artifact_file_content)
        self.artifacts = [BuildArtifact(a) for a in artifact_json]

import string
import json
from pathlib import Path
from abc import ABC, abstractmethod


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


class API(ABC):

    def __init__(self, build_info: BuildInfo) -> None:
        super().__init__()
        self.build_info = build_info

    @abstractmethod
    def upload_file(self, source: Path):
        pass

    @abstractmethod
    def create_release(self):
        pass

    def create_release(self, artifacts_file: Path):
        self.artifacts = BuildArtifacts(self.build_info)
        self.artifacts.load(artifacts_file)

        self.create_release()

    def upload_artifacts(self, artifacts_dir: Path, artifacts_file: Path):
        self.artifacts = BuildArtifacts(self.build_info)
        self.artifacts.load(artifacts_file)

        for idx, artifact in enumerate(self.artifacts.artifacts):
            file_path = artifact.get_path()

            if file_path.is_file():
                self.upload_file(artifacts_dir / file_path)
            else:
                if not artifact.optional:
                    raise RuntimeError(
                        f"Couldn't find required artifact {file_path} for upload.")

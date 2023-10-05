from pathlib import Path
from abc import ABC, abstractmethod

from hestia_ci.build_objects import *


class GenericAPI(ABC):

    def __init__(self, build_info: BuildInfo = None) -> None:
        super().__init__()
        self.build_info = build_info

    @abstractmethod
    def upload_file(self, source: Path):
        pass

    @abstractmethod
    def create_release(self):
        pass

    @abstractmethod
    def update_variable(self, key: str, val: str):
        pass

    @abstractmethod
    def create_merge_request(self, merge_json: Path):
        pass

    @abstractmethod
    def create_tag(self, tag_name: str, ref_branch: str = "master"):
        pass

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

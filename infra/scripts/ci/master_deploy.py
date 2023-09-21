import string
import json
import argparse
from pathlib import Path
import urllib
import logging

class BuildInfo():

    def __init__(self, project_name: str, version: str, arch: str) -> None:
        self.project_name = project_name
        self.version = version
        self.arch = arch
        self.artifact_dir = None

class BuildArtifact():

    def __init__(self, artifact_dict: dict) -> None:
        self.name = artifact_dict["name"] 
        self.type = artifact_dict["type"]  
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


def upload_file(source: Path, destination: Path, token: str):

    """
    Should be a detailed upload method with configurable retry (sleep time, max attempts etc)
    """

    # Or requests library if easier
    req = urllib.request.Request(destination)
    try:
        urllib.request.urlopen(req)
        return True
    except urllib.error.HTTPError as e:
        logging.error("Failed to upload file with" + str(e))
        return False

def upload_artifacts(build_info: BuildInfo, artifact_file: Path, endpoint: str, token: str):

    artifacts = BuildArtifacts(build_info)
    artifacts.load(artifact_file)

    for artifact in artifacts.artifacts:
        file_path = artifact.get_path()

        if file_path.is_file():
            if not upload_file(build_info.artifact_dir / file_path, endpoint / file_path, token):
                raise RuntimeError(f"Couldn't upload artifact {file_path}.")
        else:
            if not artifact.optional:
                raise RuntimeError(f"Couldn't find required artifact {file_path} for upload.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--artifacts_file', type=Path)
    parser.add_argument('--artifacts_dir', type=Path)
    parser.add_argument('--upload_endpoint', type=Path)
    parser.add_argument('--token', type=str)
    parser.add_argument('--project_name', type=str)
    parser.add_argument('--arch', type=str)
    parser.add_argument('--version', type=str)

    args = parser.parse_args()

    build_info = BuildInfo(args.project_name, args.version, args.arch)
    build_info.artifact_dir = args.artifacts_dir

    upload_artifacts(build_info, args.artifacts_file, args.upload_endpoint, args.token)
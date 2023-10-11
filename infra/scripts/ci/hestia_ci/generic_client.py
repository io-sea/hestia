from pathlib import Path 
from abc import ABC, abstractmethod
from typing import Dict, Any
from hestia_ci.build_objects import *

class GenericClient(ABC):
    def __init__(self) -> None:
        """
        API authentication and connection done in this method
        """
        super().__init__()

    @abstractmethod
    def upload_artifact(self, source: Path, build_info: BuildInfo) -> None:
        """
        Upload a persistent artifact via the API, given the location and build information
        """
        pass
    
    @abstractmethod
    def get_artifact_url(self, artifact: BuildArtifact, build_info: BuildInfo) -> str:
        """
        Return a URL location for an artifact, given the filename and build information
        """
        pass
    
    @abstractmethod
    def create_release(self, artifacts: BuildArtifacts, 
                       branch_ref: str = "master") -> None:
        """
        Create a release of the project from git branch branch_ref, linking assets of the 
        corresponding version from the package repository.
        """
        pass

    @abstractmethod
    def update_variable(self, key: str, val: str) -> None:
        """
        Updates a persistent CI variable, for future runs of the CI
        """
        pass
    
    @abstractmethod
    def get_variable(self, key: str) -> str | None:
        """
        Retrieve a persistent CI variable value from the API
        """
    
    @abstractmethod
    def create_merge_request(self, merge_params: Dict[str, Any], auto_merge: bool = True) -> None:
        """
        Create a merge request with parameters as specified
        Merge it immediately if auto_merge is True
        """
        pass

    @abstractmethod
    def create_tag(self, tag_name: str, ref_branch: str = "master") -> None:
        """
        Create a tag with name `tag_name`, assigned to the most recent 
        commit on `ref_brach`
        """
        pass
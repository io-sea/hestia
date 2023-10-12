import os
from pathlib import Path
from typing import Dict, Any

import gitlab

from hestia_ci.generic_client import GenericClient
from hestia_ci.build_objects import BuildInfo, BuildArtifact, BuildArtifacts
from hestia_ci.exceptions import *

class GitlabClient(GenericClient):
    def __init__(self, 
                 token: str | None = None,
                 private_token: str | None = None,
                 api_url: str | None = None,
                 project_id: str | None = None) -> None:
        """
        All parameters will be fetched automatically from the CI job's environment variables,
        except for the 'private_token' which you will need to provide manually for elevated API access
        """
        super().__init__()
        
        # Authenticate with Gitlab API
        if api_url is None:
            api_url = os.environ['CI_API_V4_URL']
        if project_id is None:
            project_id = os.environ['CI_PROJECT_ID']
        if token is None:
            token = os.environ['CI_JOB_TOKEN']

        self.gl_job = gitlab.Gitlab(
                    url=api_url, job_token=token).projects.get(project_id, lazy=True)
    
        self._gl_bot = None
        if private_token is not None:
            self._gl_bot = gitlab.Gitlab(
                            url=api_url, private_token=private_token).projects.get(project_id, lazy=True)
                
    @property
    def gl_bot(self):
        if self._gl_bot is None: 
            raise HestiaPermissionError("Elevated API access attempted without private token")
        return self._gl_bot

    def get_artifact_url(self, artifact: BuildArtifact, build_info: BuildInfo) -> str:
        return (f"{self.gl_job.generic_packages.path}"
                f"/{build_info.project_name}"
                f"/{build_info.version}"
                f"/{artifact.get_path()}")
    
    def upload_artifact(self, source: Path, build_info: BuildInfo) -> None:
        """
        Persists a build artifact to the Gitlab project's package registry.
        """        
        self.gl_job.generic_packages.upload(
            package_name=build_info.project_name,
            package_version=build_info.version,
            file_name=source.name,
            path=source
        )

    def create_release(self, artifacts: BuildArtifacts, branch_ref: str = "master") -> None:
        """
        Creates a Gitlab release page, linking assets of the 
        corresponding version from the package repository.
        """
        release = self.gl_bot.releases.create({
            "tag_name": f"v{artifacts.build_info.version}",
            "ref": branch_ref,
        })

        for artifact in artifacts.artifacts:
            release.links.create({
                "url": self.get_artifact_url(artifact, artifacts.build_info),
                "name": artifact.get_path(),
                "link_type": artifact.link_type
            })

    def update_variable(self, key: str, val: str) -> None:
        """
        Updates a persistent CI variable, for future runs of the CI
        """
        self.gl_bot.variables.update(key, {'value': val})
    
    def get_variable(self, key: str) -> str | None:
        """
        Retrieve a persistent CI variable value from the API
        """
        try:
            return self.gl_job.variables.get(key).attributes.get("value")
        except gitlab.GitlabGetError:
            raise HestiaGetError
        
    def create_merge_request(self, merge_params: Dict[str, Any], auto_merge: bool = True):
        """
        Create a merge request with given parameter dictionary, specified here:
        https://docs.gitlab.com/ee/api/merge_requests.html#create-mr
        Merge it immediately if auto_merge is True
        """
        mr = self.gl_bot.mergerequests.create(data=merge_params)
        if auto_merge:
            mr.merge()

    def create_tag(self, tag_name: str, ref_branch: str = "master"):
        """
        Create a tag with name `tag_name`, assigned to the most recent 
        commit on `ref_brach`
        """
        self.gl_bot.tags.create(
            {
                "tag_name": tag_name,
                "ref": ref_branch
            }
        )
    
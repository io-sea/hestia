import os
import json
from pathlib import Path

import gitlab

from hestia_ci.build_objects import BuildInfo
from hestia_ci.generic_api import GenericAPI
from hestia_ci.exceptions import HestiaConfigurationError


class GitlabAPI(GenericAPI):
    def __init__(self,
                 build_info: BuildInfo = None,
                 api_url: str = None,
                 project_id: str = None,
                 job_token: str = None,
                 bot_token: str = None
                 ) -> None:
        """
        Authenticates the with the Gitlab API using the python-gitlab library. 

        Note all parameters will be fetched automatically from the CI job's environment variables,
        except for the 'bot_token' which you will need to provide manually for elevated API access
        """
        super().__init__(build_info)

        if api_url is None:
            api_url = os.environ['CI_API_V4_URL'],
        if project_id is None:
            project_id = os.environ['CI_PROJECT_ID'],
        if job_token is None:
            job_token = os.environ['CI_JOB_TOKEN'],

        self.gl_job = gitlab.Gitlab(
            url=api_url, job_token=job_token).projects.get(project_id, lazy=True)
        if bot_token is not None:
            self.gl_bot = gitlab.Gitlab(
                url=api_url, private_token=bot_token).projects.get(project_id, lazy=True)
        else:
            self.gl_bot = None

    def check_build_info(self) -> None:
        if self.build_info is None:
            raise HestiaConfigurationError("Missing Build Information")

    def upload_file(self, source: Path):
        """
        Uploads a file to the GitLab project's package registry.
        """
        self.check_build_info()
        return self.gl_job.generic_packages.upload(
            package_name=self.build_info.project_name,
            package_version=str(self.build_info.version),
            file_name=source.name,
            path=source
        ).encoded_id()

    def get_generic_package_link(self, file_name: str) -> str:
        return f"""{self.self.gl_job.generic_packages.path()}
                    /{self.build_info.project_name}
                    /{self.build_info.version}
                    /{file_name}"""

    def create_release(self, branch_ref: str = "master"):
        """
        Creates a Gitlab release page, linking assets of the 
        corresponding version from the package repository.
        """
        self.check_build_info()

        release = self.gl_bot.releases.create({
            "tag_name": f"v{self.build_info.version}",
            "ref": branch_ref,
        })

        for artifact in self.artifacts.artifacts:
            release.links.create({
                "url": self.get_generic_package_link(artifact.get_path()),
                "name": artifact.name,
                "link_type": artifact.link_type,
                "direct_asset_path": f"{artifact.link_type}/{artifact.get_path()}"
            })

    def update_variable(self, key: str, val: str):
        """
        Updates a persistent CI variable, for future runs of the CI
        """
        self.gl_bot.variables.update(str(key), str(val))

    def create_merge_request(self, merge_json: Path, auto_merge: bool = True) -> dict:
        """
        Create a merge request with given parameter file, specified here:
        https://docs.gitlab.com/ee/api/merge_requests.html#create-mr
        Merge it immediately if auto_merge is True
        """
        with open(merge_json, 'r') as f:
            data = json.load(f)
            mr = self.gl_bot.mergerequests.create(data)
            if auto_merge:
                mr.merge()
            return data

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

import os
from pathlib import Path

import gitlab

import hestia_ci


class GitlabAPI(hestia_ci.API):
    def __init__(self,
                 build_info: hestia_ci.BuildInfo,
                 api_url: str = os.environ['CI_API_V4_URL'],
                 project_id: str = os.environ['CI_PROJECT_ID'],
                 job_token: str = os.environ['CI_JOB_TOKEN'],
                 bot_token: str = None
                 ) -> None:
        """
        Authenticates the with the Gitlab API using the python-gitlab library. 

        Note all parameters will be fetched automatically from the CI job's environment variables,
        except for the 'bot_token' which you will need to provide manually for elevated API access
        """
        super().__init__(build_info)

        self.gl_job = gitlab.Gitlab(
            url=api_url, job_token=job_token).projects.get(project_id, lazy=True)
        if bot_token is not None:
            self.gl_bot = gitlab.Gitlab(
                url=api_url, private_token=bot_token).projects.get(project_id, lazy=True)
        else:
            self.gl_bot = None

    def upload_file(self, source: Path):
        """
        Uploads a file to the GitLab project's package registry.
        """
        return self.gl_job.generic_packages.upload(
            package_name=self.build_info.project_name,
            package_version=self.build_info.version,
            file_name=source.name,
            path=source
        ).encoded_id()

    def create_release(self, branch_ref: str = "master"):
        """
        Creates a Gitlab release page, linking assets of the 
        corresponding version from the package repository.
        """

        release = self.gl_bot.releases.create({
            "tag_name": f"v{self.build_info.version}",
            "ref": branch_ref,
        })

        artifact_url = f"""{self.self.gl_job.generic_packages.path()}
                            /{self.build_info.project_name}
                            /{self.build_info.version}"""

        for artifact in self.artifacts.artifacts:
            release.links.create({
                "url": f"{artifact_url}/{artifact.get_path()}",
                "name": artifact.name,
                "link_type": artifact.link_type,
                "direct_asset_path": ""
            })

    def update_variable(self, key:str, val:str):
        self.gl_bot.variables.update(key, val)


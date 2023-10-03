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
        Uploads a file to the Gitlab project's package registry.
        """
        self.gl_job.generic_packages.upload(
                package_name=self.build_info.project_name, 
                package_version=self.build_info.version,
                file_name=source.name,
                path=source
            )

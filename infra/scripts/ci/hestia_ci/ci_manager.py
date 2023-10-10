import os
from pathlib import Path

from hestia_ci.generic_client import GenericClient
from hestia_ci.build_objects import BuildInfo, BuildArtifacts
from hestia_ci.utils import Version, ParsableArgs

from dataclasses import dataclass
from typing import Literal
    

class CIManager:
    def __init__(self,
                 build_info: BuildInfo,
                 client: GenericClient) -> None:
       
        self.build_info = build_info
        self.client = client
        
    def upload_artifacts(self, 
                         artifacts_file: Path, 
                         artifacts_dir: Path) -> BuildArtifacts:
            """
            Persist all build artifacts specified in the artifacts_file from the artifact_dir
            """
            artifacts = BuildArtifacts(self.build_info)
            artifacts.load(artifacts_file)

            for artifact in artifacts.artifacts:
                file_path = artifact.get_path()

                if file_path.is_file():
                    self.client.upload_artifact(artifacts_dir / file_path, self.build_info)
                else:
                    if not artifact.optional:
                        raise RuntimeError(
                            f"Couldn't find required artifact {file_path} for upload.")
                    
            return artifacts
    
    def update_variable(self, key: str, value: str):
        self.client.update_variable(key, value)

    @dataclass
    class DeployArgs(ParsableArgs):
        artifacts_file: Path   # Build artifact manifest (JSON file)
        artifacts_dir: Path     # Build artifact location
        
        # Deployment type
        schedule_type: Literal["DEPLOY_MASTER_PATCH", # Deploying the current version
                            "DEPLOY_MASTER_MINOR", # Deploying the next minor version
                            "DEPLOY_MASTER_MAJOR"] # Deploying the next major version
        
        # Optional overrides for CI defaults
        nightly_var: str = "CI_NIGHTLY_VERSION"
        patch_var: str = "CI_PATCH_RELEASE_VERSION"   
        minor_var: str = "CI_MINOR_RELEASE_VERSION"
        major_var: str = "CI_MAJOR_RELEASE_VERSION"   
        release_branch: str = "master"

    def deploy_release(self, args: DeployArgs):
        """
        Execute the following tasks in order:
         - Upload artifacts (rpms, docs, src) to the package registry
         - Create a tagged release of the requested version, linked to the artifacts uploaded
         - Increment release version numbers to be next release of that type
        """
        artifacts = self.upload_artifacts(artifacts_dir=args.artifacts_dir,
                            artifacts_file=args.artifacts_file)

        self.client.create_release(artifacts=artifacts, branch_ref=args.release_branch)
        
        version = Version.from_string(self.build_info.version)
        vers = {
                args.patch_var: version.incr_patch(), 
                args.minor_var: version.incr_minor(), 
                args.major_var: version.incr_major()
            }

        if args.schedule_type == "DEPLOY_MASTER_MINOR":
            vers[args.patch_var] = vers[args.minor_var].incr_patch()
            vers[args.minor_var] = vers[args.minor_var].incr_minor()
            
        elif args.schedule_type == "DEPLOY_MASTER_MAJOR":
            vers[args.patch_var] = vers[args.major_var].incr_patch()
            vers[args.minor_var] = vers[args.major_var].incr_minor()
            vers[args.major_var] = vers[args.major_var].incr_major()

        vers[args.nightly_var] = vers[args.patch_var]
        
        # Persist release version number variables to CI
        for key,var in vers:
            self.client.update_variable(key=key, val=var)

    @dataclass
    class MergeArgs(ParsableArgs):
        """
        All specified variables are optional overrides
        """
        source_branch: str = "devel"
        target_branch: str = "master"
        title: str = "Nightly CI Merge"
        description: str = "Merge from successful nightly CI"
        remove_source_branch: bool = False
        squash_on_merge: bool = False
        auto_merge: bool = True
        nightly_var: str = "CI_NIGHTLY_VERSION"
        patch_var: str = "CI_PATCH_RELEASE_VERSION"   
        commit_var: str = "CI_COMMIT_VAR"
        nightly_commit_var: str = "CI_LAST_NIGHTLY_COMMIT_SHA"   

    def nightly_merge(self, merge_args: MergeArgs):
        """
        Execute the following tasks in order:
         - Create and merge a merge-request with specified parameters
         - Tag the merge point in master
         - Update the patch release version (to match the tag)
         - Update most recent nightly run commit SHA to stop repeat runs
         - Increment nightly version string
        """
        self.client.create_merge_request(vars(merge_args), merge_args.auto_merge)
        self.client.create_tag(f"v{self.build_info.version}", merge_args.target_branch)
        self.client.update_variable(merge_args.patch_var, self.build_info.version)
        self.client.update_variable(merge_args.nightly_commit_var, os.environ[merge_args.commit_var])
        self.client.update_variable(merge_args.nightly_var, 
                                    str(Version.from_string(self.build_info.version).incr_patch()))
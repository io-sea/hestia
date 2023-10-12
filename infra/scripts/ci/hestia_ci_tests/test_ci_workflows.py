import os
import shutil
from pathlib import Path

import hestia_ci
from hestia_ci import build_objects

import pytest


class TestCIWorkflows:
    @pytest.fixture(scope="class")
    def build_info(self):
        return hestia_ci.BuildInfo("testid", "0.0.0", "x86_64")

    @pytest.fixture(scope="function") 
    def ci_manager(self, build_info):    
        client = hestia_ci.GitlabClient(private_token="token")
        client.gl_job = client._gl_bot # To allow for a shared backend
        ci_manager = hestia_ci.CIManager(build_info, client)
        with pytest.MonkeyPatch.context() as mp:
            mp.setenv("CI_NIGHTLY_VERSION", "0.0.1")
            mp.setenv("CI_PATCH_RELEASE_VERSION", "0.0.0")
            mp.setenv("CI_MINOR_RELEASE_VERSION", "0.1.0")
            mp.setenv("CI_MAJOR_RELEASE_VERSION", "1.0.0")

            mp.setenv("CI_COMMIT_VAR", "fakesha1")
            mp.setenv("CI_LAST_NIGHTLY_COMMIT_SHA", "fakesha2")
            
            yield ci_manager

    @pytest.fixture(scope="class")
    def data_dir(self, tmp_path_factory: pytest.TempPathFactory):
        data_dir = tmp_path_factory.mktemp("workflow_test_data")
        return data_dir
    
    @pytest.fixture(scope="class")
    def artifact_dir(self, data_dir):
        artifact_file = Path("infra/scripts/ci/hestia_ci_tests/data/test_artifacts.json")
        if not artifact_file.exists():
            raise FileNotFoundError(artifact_file)
        shutil.copyfile(artifact_file, data_dir/artifact_file.name)

        return data_dir

    @pytest.fixture(scope="class") 
    def artifacts(self, artifact_dir, build_info):
        artifacts = build_objects.BuildArtifacts(build_info)
        artifacts.load(artifact_dir / "test_artifacts.json")
        
        for artifact in artifacts.artifacts:
            if not artifact.optional:
                with open(artifact_dir / artifact.get_path(), "w") as f:
                    f.write("testartifact")
        
        return artifacts

  
    def test_nightly_merge(self, ci_manager: hestia_ci.CIManager):
        merge_args = ci_manager.MergeArgs(
            title="test merge",
            description="test merge",
            source_branch="testbrancha",
            target_branch="testbranchb",
            remove_source_branch=True,
            auto_merge=False,
            squash=False
            )
        ci_manager.nightly_merge(merge_args)
        from hestia_ci_tests.mock_gitlab import MockProject
        gl: MockProject = ci_manager.client.gl_bot # type: ignore
        assert gl.mergerequests.get(merge_args.title).attributes == vars(merge_args)
        assert gl.tags.get(f"v{ci_manager.build_info.version}") is not None
        assert gl.variables.get(merge_args.nightly_commit_var).attributes["value"] == os.environ[merge_args.commit_var]
        assert gl.variables.get(merge_args.patch_var).attributes["value"] == ci_manager.build_info.version
        assert gl.variables.get(merge_args.nightly_var).attributes["value"] == "0.0.1"
    
    def test_release_deploy(self, ci_manager: hestia_ci.CIManager, 
                            artifact_dir, artifacts: build_objects.BuildArtifacts):
        deploy_args = ci_manager.DeployArgs(
            artifacts_dir=artifact_dir,
            artifacts_file=artifact_dir / "test_artifacts.json",
            schedule_type="DEPLOY_MASTER_PATCH",
            release_branch="testbranch"
            )        
        ci_manager.deploy_release(deploy_args)
        from hestia_ci_tests.mock_gitlab import MockProject
        gl: MockProject = ci_manager.client.gl_bot # type: ignore
        package = gl.generic_packages.get(artifacts.build_info.project_name)\
                    .attributes[artifacts.build_info.version]
        release_links = gl.releases.get(f"v{artifacts.build_info.version}").links

        for artifact in artifacts.artifacts:
            if not artifact.optional:
                assert package[str(artifact.get_path())] \
                    == artifact_dir / artifact.get_path()
                assert release_links.get(str(artifact.get_path())) is not None
        exp_versions = ["0.0.1", "0.0.1", "0.1.0", "1.0.0"] 
        exp_ver_vars = [ 
            deploy_args.nightly_var,
            deploy_args.patch_var,
            deploy_args.minor_var,
            deploy_args.major_var
        ]
        assert [ gl.variables.get(v).attributes["value"] for v in exp_ver_vars]\
              == exp_versions
        
        deploy_args.schedule_type = "DEPLOY_MASTER_MINOR"
        exp_versions = ["0.1.1", "0.1.1", "0.2.0", "1.0.0"] 
        ci_manager.deploy_release(deploy_args)
        assert [ gl.variables.get(v).attributes["value"] for v in exp_ver_vars]\
              == exp_versions
        
        deploy_args.schedule_type = "DEPLOY_MASTER_MAJOR"
        exp_versions = ["1.0.1", "1.0.1", "1.1.0", "2.0.0"] 
        ci_manager.deploy_release(deploy_args)
        assert [ gl.variables.get(v).attributes["value"] for v in exp_ver_vars]\
              == exp_versions
        
    
                
        
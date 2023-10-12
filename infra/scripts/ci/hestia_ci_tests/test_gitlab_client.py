import pytest
import hestia_ci
import copy

class TestGitlabClient:
    # Determines if the MockGitlab context is persisted within the class
    @pytest.fixture(scope="class") 
    def gl_client(self) -> hestia_ci.GitlabClient:    
        client = hestia_ci.GitlabClient(private_token="token")
        client.gl_job = client._gl_bot # To allow for a shared backend
        return client
    
    @pytest.fixture(scope="class")
    def build_info(self):
        return hestia_ci.BuildInfo("testid", "0.0.0", "x86_64")
          
    @pytest.fixture(scope="class")
    def build_artifact(self):
        from hestia_ci.build_objects import BuildArtifact

        return BuildArtifact(
            name = "my_artifact",
            type = "txt",
            description= "test artifact" 
        )
    
    @pytest.fixture(scope="class")
    def build_artifacts(self, build_info, build_artifact):
        from hestia_ci.build_objects import BuildArtifacts
        artifacts = BuildArtifacts(build_info)

        for i in range(3):
            artifacts.artifacts.append(copy.deepcopy(build_artifact))
            artifacts.artifacts[i].name += f"_{i}"
   
        return artifacts
        
    def test_authentication(self):
        client = hestia_ci.GitlabClient()
        assert client.gl_job is not None
        
        with pytest.raises(hestia_ci.exceptions.HestiaPermissionError):
            assert client.gl_bot is None    
        
        client = hestia_ci.GitlabClient(private_token="token")
        assert client.gl_bot is not None

    def test_variables(self, gl_client: hestia_ci.GitlabClient):
        key, val = "my_key", "my_value"
        with pytest.raises(hestia_ci.exceptions.HestiaGetError):
            gl_client.get_variable(key)
        
        gl_client.update_variable(key, val)
        assert gl_client.get_variable(key) == val

    def test_artifacts(self, gl_client: hestia_ci.GitlabClient, build_info, build_artifact):
        gl_client.upload_artifact(build_artifact.get_path(), build_info)
        
        package = gl_client.gl_bot.generic_packages.get(build_info.project_name).attributes
        assert package is not None
        assert package[build_info.version][str(build_artifact.get_path())]\
            == build_artifact.get_path()

        assert gl_client.get_artifact_url(build_artifact, build_info) == f"https://gitlab.example.com/generic_packages/{build_info.project_name}/{build_info.version}/{build_artifact.get_path()}"

    def test_releases(self, gl_client: hestia_ci.GitlabClient, build_artifacts):
        gl_client.create_release(build_artifacts, "testbranch")
        release = gl_client.gl_bot.releases.get(f"v{build_artifacts.build_info.version}")
        assert release is not None
        assert release.attributes["tag_name"] == f"v{build_artifacts.build_info.version}"
        assert release.attributes["ref"] == "testbranch"

        assert len(release.links.list()) == len(build_artifacts.artifacts)
        for artifact, link in zip(build_artifacts.artifacts, release.links.list()):
            assert link.attributes["url"] == gl_client.get_artifact_url(artifact, build_artifacts.build_info)
            assert link.attributes["name"] == artifact.get_path()
            assert link.attributes["link_type"] == artifact.link_type

    def test_tags(self, gl_client: hestia_ci.GitlabClient):
        gl_client.create_tag("testtag", "testbranch")
        tag = gl_client.gl_bot.tags.get("testtag")
        assert tag.attributes["tag_name"] == "testtag"
        assert tag.attributes["ref"] == "testbranch"
        

    def test_merge_requests(self, gl_client: hestia_ci.GitlabClient):
        merge_params = {
            "source_branch": "testbrancha",
            "target_branch": "testbranchb",
            "title": "testmr",
            "description": "",
            "remove_source_branch": False,
            "squash": False
        }
        gl_client.create_merge_request(merge_params, auto_merge=True)
        assert gl_client.gl_bot.mergerequests.get("testmr").attributes == merge_params
import pytest
import hestia_ci

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
          
    def test_authentication(self):
        client = hestia_ci.GitlabClient()
        assert client.gl_job is not None
        
        with pytest.raises(hestia_ci.exceptions.HestiaPermissionError):
            assert client.gl_bot is None    
        
        client = hestia_ci.GitlabClient(private_token="token")
        assert client.gl_bot is not None

    def test_update_variable(self, gl_client: hestia_ci.GitlabClient):
        key, val = "my_key", "my_value"
        gl_client.update_variable(key, val)
        assert gl_client.get_variable(key) == val

    def test_artifacts(self, gl_client: hestia_ci.GitlabClient, build_info):
        from hestia_ci.build_objects import BuildArtifact

        artifact = BuildArtifact(
            name = "my_artifact",
            type = "txt",
            description= "" 
            )
        
        gl_client.upload_artifact(artifact.get_path(), build_info)
        
        assert isinstance(gl_client.gl_bot.generic_packages.get(build_info.project_name), dict)

        assert gl_client.get_artifact_url(artifact, build_info) == f"https://gitlab.example.com/MockGenericPackageManager/{build_info.project_name}/{build_info.version}/{artifact.get_path()}"
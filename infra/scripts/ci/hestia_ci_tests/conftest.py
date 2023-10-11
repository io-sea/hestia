import pytest 

@pytest.fixture(scope="module")
def monkeymodule():
    with pytest.MonkeyPatch.context() as mp:
        yield mp

@pytest.fixture(scope="module", autouse=True)
def mock_gitlab(monkeymodule):
    from hestia_ci_tests.mock_gitlab import MockGitlab
    monkeymodule.setattr("gitlab.Gitlab", MockGitlab)
    # Set dummy Gitlab CI environment variables
    monkeymodule.setenv("CI_API_V4_URL", "https://gitlab.example.com")
    monkeymodule.setenv("CI_PROJECT_ID", "testid")
    monkeymodule.setenv("CI_JOB_TOKEN", "test-token")
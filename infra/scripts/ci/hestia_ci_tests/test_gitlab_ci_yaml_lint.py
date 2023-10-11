from pathlib import Path

import pytest

# Allow the use of a real gitlab api here
@pytest.fixture(autouse=True)
def mock_gitlab():
    return

@pytest.fixture(scope='session')
def real_gitlab_client(pytestconfig):
    """
    Returns a connected Gitlab client, given a private token. 
    Skips test if no token is provided. 
    """
    import json
    auth_path = Path("infra/scripts/ci/gitlab_ci_auth.json")
    if not auth_path.exists():
        pytest.skip("No connection details given for Gitlab, skipping")
    with open(auth_path) as f:
        auth = json.load(f)

    if auth["gitlab_private_token"] == "YOUR_AUTH_TOKEN":
        pytest.skip("No private token given for Gitlab, skipping")

    from gitlab import Gitlab
    gl_client = Gitlab(url=auth["gitlab_api_url"], private_token=auth["gitlab_private_token"])
    yield gl_client.projects.get(auth["gitlab_project_id"], lazy=True)

def test_gitlab_ci_yaml(real_gitlab_client):
    """
    Lints the Gitlab CI yaml file passed against the projects API endpoint
    """
    yml_path = Path(".gitlab-ci.yml")
    if not yml_path.exists():
        raise FileNotFoundError(yml_path)
    with open(yml_path) as f:
        gitlab_ci_yml = f.read()
    real_gitlab_client.ci_lint.validate({"content": gitlab_ci_yml})
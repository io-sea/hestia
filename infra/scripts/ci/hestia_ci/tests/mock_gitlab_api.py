from hestia_ci.generic_client import GenericClient

class MockGitlabClient(GenericClient):
    def __init__(self) -> None:
        super().__init__()
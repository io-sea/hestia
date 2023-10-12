from typing import Any, Dict
from pathlib import Path

import gitlab

class MockGitlab:
    def __init__(self,
        url: str | None = None,
        private_token: str | None = None,
        job_token: str | None = None,
        **kwargs: Any
        ) -> None:
        """
        Store all variables passed
        """
        self.url = url
        self.private_token = private_token
        self.job_token = job_token
        self.xargs = {}
        for key,val in kwargs: 
            self.xargs[key] = val
        
        self.projects = MockResourceManager(self, MockProject, "projects") 
        self.projects.create({"id": "testid"})


class MockResource:
    def __init__(self, 
                 manager, 
                 data: Dict[str, Any]) -> None:
        self.manager = manager
        self.attributes = data


class MockProject(MockResource):
    def __init__(self, manager, id) -> None:
        super().__init__(manager, id)
        self.variables = MockResourceManager(manager.gl, name="variables")
        self.generic_packages = MockGenericPackageManager(manager.gl, name="generic_packages")
        self.releases = MockResourceManager(manager.gl, MockRelease, name="releases")
        self.mergerequests = MockResourceManager(manager.gl, MockMerge, name="mergerequests")
        self.tags = MockResourceManager(manager.gl, name="tags")


class MockRelease(MockResource):
    def __init__(self, manager, data: Dict[str, Any] | None = None) -> None:
        super().__init__(manager, data)
        self.links = MockResourceManager(manager.gl, name=f"{manager.name}/links")


class MockMerge(MockResource):
    def merge(self):
        return None 


class MockResourceManager:
    def __init__(self, gl: MockGitlab, 
                 resource_cls = MockResource, 
                 name: str | None = None) -> None:
        self.gl = gl
        self.name = name
        self.path = f"{gl.url}/{self.__class__.__name__ if self.name is None else self.name}"
        self._obj_store = {}
        self.resource_cls = resource_cls

    def create(self, data: Dict[str, Any]) -> MockResource:
        id_keys = ["id", "name", "tag_name", "title", "package_name"]
        id_key = "id"
        for key in id_keys:
            if key in data.keys():
                id_key = key
                break
        if data.get(id_key) is not None:
            id = str(data[id_key])
            self._obj_store[id] = self.resource_cls(self, data)
            return self._obj_store[id]
        else:
            return self.resource_cls(self, data)
        
    def get(self, id: str, lazy: bool = True) -> MockResource:
        try:
            return self._obj_store[id]
        except KeyError:
            raise gitlab.GitlabGetError(id)
        
    def list(self):
        return list(self._obj_store.values())
        
    def update(self, id: str, value: Dict[str, Any]) -> None:
        self._obj_store[id] = self.resource_cls(self, value)


class MockGenericPackageManager(MockResourceManager):    
    def upload(self,
        package_name: str,
        package_version: str,
        file_name: str,
        path: str | Path):
        if package_name not in self._obj_store:
            package = self.create({"package_name": package_name})
        else: 
            package = self.get(package_name)

        if package_version not in package.attributes:
            package.attributes[package_version] = {}

        package.attributes[package_version][file_name] = path
    
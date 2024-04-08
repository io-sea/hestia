import argparse
import json
import logging
import urllib.request


logger = logging.getLogger(__name__)

_DRY_RUN = False


class ReleaseAssetCollection():

    def __init__(self, project_name, project_version, plugins) -> None:
        self.arch = "x86_64"
        self.plugins = plugins
        self.rpm_epoch = "1"
        rpm_suffix = f"{project_version}-{self.rpm_epoch}.{self.arch}.rpm"
        self.packages = [f"{project_name}-{rpm_suffix}",
                         f"{project_name}-devel-{rpm_suffix}",
                         f"{project_name}-debuginfo-{rpm_suffix}",
                         f"{project_name}-{project_version}-{self.rpm_epoch}.src.rpm",
                         f"{project_name}-docs-{project_version}.tar.gz"
                         ]

        for plugin in self.plugins:
            self.packages.append(f"{project_name}-{plugin}-{rpm_suffix}")


class ReleaseAssetLink():

    def __init__(self, name, base_url) -> None:
        self.name = name
        self.url = f"{base_url}/{name}"
        self.direct_asset_path = f"/binaries/{name}"
        self.link_type = "package"

    def serialize(self):
        return {"name": self.name,
                "url": self.url,
                "direct_asset_path": self.direct_asset_path,
                "link_type": self.link_type}


class ReleaseManifest():

    def __init__(self, project_version: str, base_url: str,
                 assets: ReleaseAssetCollection) -> None:
        self.name = f"Release {project_version}"
        self.tag_name = f"v{project_version}"
        self.ref = "master"
        self.assets = assets

        self.asset_links: list = []
        for package in self.assets.packages:
            self.asset_links.append(ReleaseAssetLink(package, base_url))

    def serialize(self):

        return {"name": self.name,
                "tag_name": self.tag_name,
                "ref": self.ref,
                "assets": {"links": [a.serialize() for a in self.asset_links]}}


def get_manifest(registry_url, project_version, release_assets):

    manifest = ReleaseManifest(project_version, registry_url,
                               release_assets)
    return json.dumps(manifest.serialize(), indent=4)


def deploy(endpoint, manifest, token):

    logging.info(f"Uploading manifest to: {endpoint}")

    req = urllib.request.Request(url=endpoint,
                                 data=manifest.encode('utf-8'),
                                 method='POST')
    req.add_header('PRIVATE-TOKEN', token)
    req.add_header('Content-Type', "application/json")

    if _DRY_RUN:
        logging.info(f"Making request: {req}")
    else:
        response = urllib.request.urlopen(req)
        content = response.read()
        content = content.decode('utf-8')
        logger.info(content)

    logging.info("Finished uploading manifest")


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("action", type=str)
    parser.add_argument("--project_name", type=str, default="hestia",
                        help="Name of project")
    parser.add_argument("--project_version", type=str,
                        default="0.0.0",
                        help="Project version")
    parser.add_argument("--project_ci_id", type=str, default="418",
                        help="Project id on gitlab")
    parser.add_argument("--ci_api_endpoint", type=str, 
                        default="https://git.ichec.ie/api/v4",
                        help="Gitlab CI api endpoint")
    parser.add_argument("--plugin_names", type=str,
                        help="List of project plugins")
    parser.add_argument("--ci_token", type=str,
                        help="Token for deploying to CI")
    parser.add_argument("--dry_run", type=bool, default=False,
                        help="Don't make any real changes if True")
    args = parser.parse_args()

    ci_url = f"{args.ci_api_endpoint}/projects/{args.project_ci_id}"
    ci_package_registry_url = f"{ci_url}/packages/generic"
    package_url = f"{ci_package_registry_url}/{args.project_name}/{args.project_version}"

    if args.plugin_names:
        plugins = args.plugin_names.split(",")
    else:
        plugins = []

    _DRY_RUN = args.dry_run

    release_assets = ReleaseAssetCollection(args.project_name,
                                            args.project_version,
                                            plugins)

    manifest = get_manifest(package_url, args.project_version, release_assets)

    if args.action == "create_manifest":
        with open("manifest.json", 'w') as f:
            f.write(manifest)
    elif args.action == "deploy":
        logging.basicConfig(encoding='utf-8', level=logging.INFO)

        endpoint_url = f"{ci_url}/releases"
        deploy(endpoint_url, manifest, args.ci_token)
import argparse
from pathlib import Path

from hestia_ci import hestia_ci
from hestia_ci.gitlab_api import GitlabAPI


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--artifacts_file', type=Path)
    parser.add_argument('--artifacts_dir', type=Path)

    parser.add_argument('--project_name', type=str)
    parser.add_argument('--arch', type=str)
    parser.add_argument('--version', type=str)

    parser.add_argument('--bot_token', type=str)

    args = parser.parse_args()

    build_info = hestia_ci.BuildInfo(args.project_name, args.version, args.arch)

    api = GitlabAPI(build_info=build_info, bot_token=args.bot_token)

    api.upload_artifacts(artifacts_dir=args.artifacts_dir, artifacts_file=args.artifacts_file)

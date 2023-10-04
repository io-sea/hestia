import argparse
from pathlib import Path


from hestia_ci import hestia_ci
from hestia_ci.gitlab_api import GitlabAPI


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--project_name', type=str)
    parser.add_argument('--arch', type=str)
    parser.add_argument('--version', type=str)

    parser.add_argument('--bot_token', type=str)

    parser.add_argument("key", type=str)
    parser.add_argument("value", type=str)
    
    args = parser.parse_args()

    build_info = hestia_ci.BuildInfo(
        args.project_name, args.version, args.arch)

    api = GitlabAPI(build_info=build_info, bot_token=args.bot_token)

    api.update_variable(args.key, args.value)

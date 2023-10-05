import os
import argparse
from pathlib import Path

from hestia_ci import GitlabAPI, BuildInfo


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--artifacts_file', type=Path)
    parser.add_argument('--artifacts_dir', type=Path)

    parser.add_argument('--project_name', type=str)
    parser.add_argument('--arch', type=str)
    parser.add_argument('--version', type=str)

    parser.add_argument('--bot_token', type=str)

    parser.add_argument('--schedule_type',
                        choices=[
                            "DEPLOY_MASTER_PATCH",
                            "DEPLOY_MASTER_MINOR",
                            "DEPLOY_MASTER_MAJOR"
                        ])

    parser.add_argument('--patch_var', type=str,
                        default="CI_PATCH_RELEASE_VERSION")
    parser.add_argument('--minor_var', type=str,
                        default="CI_MINOR_RELEASE_VERSION")
    parser.add_argument('--major_var', type=str,
                        default="CI_MAJOR_RELEASE_VERSION")

    parser.add_argument('--release_branch', type=str, default="master")

    args = parser.parse_args()

    build_info = BuildInfo(args.project_name, args.version, args.arch)

    api = GitlabAPI(build_info=build_info, bot_token=args.bot_token)

    # Upload artifacts (rpms, docs, src) to the package registry
    api.upload_artifacts(artifacts_dir=args.artifacts_dir,
                         artifacts_file=args.artifacts_file)

    # Create a release of the requested version, linked to the artifacts uploaded
    api.create_release(branch_ref=args.release_branch)

    # Increment release version numbers to be next release of that type
    patch_ver = int(os.environ[args.patch_var].split(".", 3)[2])
    minor_ver = int(os.environ[args.minor_var].split(".", 2)[1])
    major_ver = int(os.environ[args.major_var].split(".", 1)[0])

    if args.schedule_type == "DEPLOY_MASTER_PATCH":
        patch_ver += 1

    elif args.schedule_type == "DEPLOY_MASTER_MINOR":
        minor_ver += 1
        patch_ver = 1

    elif args.schedule_type == "DEPLOY_MASTER_MAJOR":
        major_ver += 1
        minor_ver = 1
        patch_ver = 1

    # Persist release version number variables to CI
    api.update_variable(key=args.patch_var,
                        val=f"{major_ver-1}.{minor_ver-1}.{patch_ver}")
    api.update_variable(key=args.minor_var, val=f"{major_ver-1}.{minor_ver}.0")
    api.update_variable(key=args.major_var, val=f"{major_ver}.0.0")

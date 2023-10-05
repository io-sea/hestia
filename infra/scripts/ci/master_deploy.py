import os
import argparse
from pathlib import Path

from hestia_ci import GitlabAPI, BuildInfo
from hestia_ci.utils import Version


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

    parser.add_argument('--nightly_var', type=str,
                        default="CI_NIGHTLY_VERSION")
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

    # Create a tagged release of the requested version, linked to the artifacts uploaded
    api.create_release(branch_ref=args.release_branch)

    # Increment release version numbers to be next release of that type
    patch_ver = Version.from_string(os.environ[args.patch_var]) # *.*.*
    minor_ver = Version.from_string(os.environ[args.minor_var]) # *.*.0
    major_ver = Version.from_string(os.environ[args.major_var]) # *.0.0

    if args.schedule_type == "DEPLOY_MASTER_PATCH":
        patch_ver = patch_ver.incr_patch()

    elif args.schedule_type == "DEPLOY_MASTER_MINOR":
        patch_ver = minor_ver.incr_patch()
        minor_ver = minor_ver.incr_minor()
        
    elif args.schedule_type == "DEPLOY_MASTER_MAJOR":
        minor_ver = major_ver.incr_minor()
        patch_ver = major_ver.incr_patch()
        major_ver = major_ver.incr_major()

    # Persist release version number variables to CI
    api.update_variable(key=args.nightly_var, val=patch_ver)
    api.update_variable(key=args.patch_var, val=patch_ver)
    api.update_variable(key=args.minor_var, val=minor_ver)
    api.update_variable(key=args.major_var, val=major_ver)
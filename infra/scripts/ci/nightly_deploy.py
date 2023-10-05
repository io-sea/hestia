import os
import argparse
from pathlib import Path

from hestia_ci import GitlabAPI, BuildInfo
from hestia_ci.utils import Version


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('--project_name', type=str)
    parser.add_argument('--arch', type=str)
    parser.add_argument('--version', type=str)

    parser.add_argument('--bot_token', type=str)

    parser.add_argument('--merge_json', type=Path)
    
    parser.add_argument('--nightly_var', type=str,
                        default="CI_NIGHTLY_VERSION")
    parser.add_argument('--patch_var', type=str,
                        default="CI_PATCH_RELEASE_VERSION")    
    parser.add_argument('--commit_var', type=str,
                        default="CI_COMMIT_SHA")
    parser.add_argument('--nightly_commit_var', type=str,
                        default="CI_LAST_NIGHTLY_COMMIT_SHA")

    args = parser.parse_args()

    build_info = BuildInfo(args.project_name, args.version, args.arch)
    api = GitlabAPI(build_info=build_info, bot_token=args.bot_token) 

    # Create and merge a merge-request with specified parameters
    api.create_merge_request(args.merge_json, auto_merge=True)

    # Tag the merge point in master
    api.create_tag(f"v{args.version}", "master")

    # Update the patch release version (to match the tag)
    api.update_variable(args.patch_var, args.version)

    # Update most recent nightly run commit SHA to stop repeat runs
    api.update_variable(args.nightly_commit_var, os.environ[args.commit_var])

    # Increment nightly version string
    api.update_variable(args.nightly_var, Version(args.version).incr_patch)
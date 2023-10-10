import argparse

from hestia_ci import CIManager
from hestia_ci.utils import Version

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--bot_token", type=str, required=True)
    
    parser.add_argument("-ver_inc","--version_increment", choices=["patch", "minor", "major"])
    parser.add_argument("key", type=str)
    parser.add_argument("value", type=str)
    
    args = parser.parse_args()

    ci_manager = CIManager(bot_token=args.bot_token)
    
    if args.version_increment == "patch":
        ci_manager.update_variable(args.key, Version.from_string(args.value).incr_patch())
    if args.version_increment == "minor":
        ci_manager.update_variable(args.key, Version.from_string(args.value).incr_minor())
    if args.version_increment == "major":
        ci_manager.update_variable(args.key, Version.from_string(args.value).incr_major())
    else:
        ci_manager.update_variable(args.key, args.value)


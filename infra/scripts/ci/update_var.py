import argparse

from hestia_ci import GitlabAPI
from hestia_ci.utils import Version

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--bot_token', type=str)
    
    parser.add_argument('-p','--version_increment_patch', action='store_true')
    parser.add_argument('-m','--version_increment_minor', action='store_true')
    parser.add_argument('-M','--version_increment_major', action='store_true')
    
    parser.add_argument("key", type=str)
    parser.add_argument("value", type=str)
    
    args = parser.parse_args()

    api = GitlabAPI(bot_token=args.bot_token)
    
    if args.version_increment_patch:
        api.update_variable(args.key, Version.from_string(args.value).incr_patch())
    elif args.version_increment_minor:
        api.update_variable(args.key, Version.from_string(args.value).incr_minor())
    elif args.version_increment_major:
        api.update_variable(args.key, Version.from_string(args.value).incr_major())
    else:
        api.update_variable(args.key, args.value)


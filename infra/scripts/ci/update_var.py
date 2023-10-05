import argparse
from pathlib import Path


from hestia_ci import GitlabAPI


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--bot_token', type=str)

    parser.add_argument("key", type=str)
    parser.add_argument("value", type=str)
    
    args = parser.parse_args()

    api = GitlabAPI(bot_token=args.bot_token)

    api.update_variable(args.key, args.value)

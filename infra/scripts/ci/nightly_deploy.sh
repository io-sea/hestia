#!/usr/bin/env sh

# Merge devel to master
curl --request POST -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/merge_requests" \
    -H "Content-Type: application/json" \
    --data '{
        "source_branch": "devel",
        "target_branch": "master",
        "title": "Nightly CI Merge",
        "description": "Merge from successful nightly CI",
        "remove_source_branch": false,
        "squash_on_merge": false
      }' \
    -o merge.json
sleep 60
curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/merge_requests/$(jq .iid merge.json)/merge"
sleep 60

# Tag MR with version number, update patch version
curl --request POST -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
      "$HESTIA_API_URL/repository/tags?tag_name=v$CI_NIGHTLY_VERSION&ref=master"
curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_PATCH_RELEASE_VERSION" \
    --form "value=$CI_NIGHTLY_VERSION"

# Stops repeat runs
curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
      "$HESTIA_API_URL/variables/CI_LAST_NIGHTLY_COMMIT_SHA" \
      --form "value=$CI_COMMIT_SHA"


# Increment nightly version
new_nightly_ver=$(echo $CI_NIGHTLY_VERSION | awk -F. -v OFS=. '{$NF += 1 ; print}')
curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_NIGHTLY_VERSION" \
    --form "value=$new_nightly_ver"

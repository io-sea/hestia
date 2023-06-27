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
sleep 5
curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/merge_requests/$(jq .iid merge.json)/merge"
sleep 5

# Tag MR with minor patch
curl --request POST -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
      "$HESTIA_API_URL/repository/tags?tag_name=v$CI_RELEASE_VERSION&ref=master"
    
# Update last successful nightly build SHA
curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_LAST_NIGHTLY_COMMIT_SHA" \
    --form "value=$CI_COMMIT_SHA"

# Increment nightly minor version
new_nightly_ver=$(echo $CI_NIGHTLY_VERSION | awk -F. -v OFS=. '{$NF += 1 ; print}')

curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_NIGHTLY_VERSION" \
    --form "value=$new_nightly_ver"
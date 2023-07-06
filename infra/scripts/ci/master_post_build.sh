#!/usr/bin/env sh

new_nightly_ver=$(echo $CI_NIGHTLY_VERSION | awk -F. -v OFS=. '{$NF += 1 ; print}')

curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_NIGHTLY_VERSION" \
    --form "value=$new_nightly_ver"
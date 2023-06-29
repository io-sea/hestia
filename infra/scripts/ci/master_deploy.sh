#!/usr/bin/env sh

# Upload packages to registry
registry_url="$HESTIA_API_URL/packages/generic/hestia/$CI_RELEASE_VERSION/hestia-$CI_RELEASE_VERSION"
while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url.rpm" \
    --upload-file $CMAKE_BUILD_DIR/*Linux.rpm
do echo "will retry in 2 seconds"; sleep 2; done
while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url.tar.gz" \
    --upload-file $CMAKE_BUILD_DIR/*Linux.tar.gz
do echo "will retry in 2 seconds"; sleep 2; done

while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url.src.rpm" \
    --upload-file $CMAKE_BUILD_DIR/*.src.rpm
do echo "will retry in 2 seconds"; sleep 2; done
while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url-Source.tar.gz" \
    --upload-file $CMAKE_BUILD_DIR/*Source.tar.gz
do echo "will retry in 2 seconds"; sleep 2; done

# Create new release and link packages
curl --request POST -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
      "$HESTIA_API_URL/releases" \
      -H "Content-Type: application/json" \
      --data "{
        \"name\": \"Release $CI_RELEASE_VERSION\",
        \"tag_name\": \"v$CI_RELEASE_VERSION\",
        \"ref\": \"master\",
        \"assets\": { \"links\": [
          {
            \"name\": \"Hestia RPM\",
            \"url\": \"$registry_url.rpm\",
            \"direct_asset_path\": \"/binaries/hestia-$CI_RELEASE_VERSION.rpm\",
            \"link_type\": \"package\"
          },
          {
            \"name\": \"Hestia tarball\",
            \"url\": \"$registry_url.tar.gz\",
            \"direct_asset_path\": \"/binaries/hestia-$CI_RELEASE_VERSION.tar.gz\",
            \"link_type\": \"package\"
          }
        ]}
      }"

# Increment next minor release version
new_minor_ver=$(echo $CI_RELEASE_VERSION | awk -F. -v OFS=. '{$2 += 1 ; print}')
# Update nightly version to REL.1
new_nightly_ver=$(echo $CI_RELEASE_VERSION | awk -F. -v OFS=. '{$NF = 1; print}')

curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_RELEASE_VERSION" \
    --form "value=$new_minor_ver"

curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_NIGHTLY_VERSION" \
    --form "value=$new_nightly_ver"
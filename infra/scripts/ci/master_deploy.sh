#!/usr/bin/env sh

# Upload packages to registry
registry_url="$HESTIA_API_URL/packages/generic/hestia/$HESTIA_PROJECT_VERSION/hestia-$HESTIA_PROJECT_VERSION"

# RPMs: main, devel, tests, source
while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url.rpm" \
    --upload-file $CMAKE_BUILD_DIR/hestia-$HESTIA_PROJECT_VERSION-1.x86_64.rpm
do echo "will retry in 2 seconds"; sleep 2; done

while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url-devel.rpm" \
    --upload-file $CMAKE_BUILD_DIR/hestia-devel-$HESTIA_PROJECT_VERSION-1.x86_64.rpm
do echo "will retry in 2 seconds"; sleep 2; done

while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url-tests.rpm" \
    --upload-file $CMAKE_BUILD_DIR/hestia-tests-$HESTIA_PROJECT_VERSION-1.x86_64.rpm
do echo "will retry in 2 seconds"; sleep 2; done

while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url.src.rpm" \
    --upload-file $CMAKE_BUILD_DIR/hestia-$HESTIA_PROJECT_VERSION-1.x86_64..src.rpm
do echo "will retry in 2 seconds"; sleep 2; done

# TARs: main, source
while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url.tar.gz" \
    --upload-file $CMAKE_BUILD_DIR/*Linux.tar.gz
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
        \"name\": \"Release $HESTIA_PROJECT_VERSION\",
        \"tag_name\": \"v$HESTIA_PROJECT_VERSION\",
        \"ref\": \"master\",
        \"assets\": { \"links\": [
          {
            \"name\": \"Hestia RPM\",
            \"url\": \"$registry_url.rpm\",
            \"direct_asset_path\": \"/binaries/hestia-$HESTIA_PROJECT_VERSION.rpm\",
            \"link_type\": \"package\"
          },
          {
            \"name\": \"Hestia Development Headers RPM\",
            \"url\": \"$registry_url-devel.rpm\",
            \"direct_asset_path\": \"/binaries/hestia-$HESTIA_PROJECT_VERSION-devel.rpm\",
            \"link_type\": \"package\"
          },
          {
            \"name\": \"Hestia tarball\",
            \"url\": \"$registry_url.tar.gz\",
            \"direct_asset_path\": \"/binaries/hestia-$HESTIA_PROJECT_VERSION.tar.gz\",
            \"link_type\": \"package\"
          },
          {
            \"name\": \"Hestia SRC RPM\",
            \"url\": \"$registry_url.src.rpm\",
            \"direct_asset_path\": \"/binaries/hestia-$HESTIA_PROJECT_VERSION.src.rpm\",
            \"link_type\": \"package\"
          },
          {
            \"name\": \"Hestia Tests RPM\",
            \"url\": \"$registry_url-tests.rpm\",
            \"direct_asset_path\": \"/binaries/hestia-$HESTIA_PROJECT_VERSION-tests.rpm\",
            \"link_type\": \"package\"
          },
        ]}
      }"


# Update version numbers
new_nightly_ver=$CI_NIGHTLY_VERSION
new_patch_ver=$CI_PATCH_RELEASE_VERSION
new_minor_ver=$CI_MINOR_RELEASE_VERSION
new_major_ver=$CI_MAJOR_RELEASE_VERSION

if [ $CI_SCHEDULE_TYPE = "DEPLOY_MASTER_PATCH" ]; then
  new_patch_ver=$CI_NIGHTLY_VERSION
fi
if [ $CI_SCHEDULE_TYPE = "DEPLOY_MASTER_MINOR" ]; then
  new_nightly_ver=$(echo $CI_MINOR_RELEASE_VERSION | awk -F. -v OFS=. '{$NF = 1; print}')
  new_patch_ver=$new_nightly_ver
  new_minor_ver=$(echo $CI_MINOR_RELEASE_VERSION | awk -F. -v OFS=. '{$2 += 1 ; print}')
fi
if [ $CI_SCHEDULE_TYPE = "DEPLOY_MASTER_MAJOR" ]; then
  new_nightly_ver=$(echo $CI_MAJOR_RELEASE_VERSION | awk -F. -v OFS=. '{$NF = 1; print}')
  new_patch_ver=$new_nightly_ver
  new_minor_ver=$(echo $CI_MAJOR_RELEASE_VERSION | awk -F. -v OFS=. '{$2 += 1 ; print}')
  new_major_ver=$(echo $CI_MAJOR_RELEASE_VERSION | awk -F. -v OFS=. '{$1 += 1 ; print}')
fi

curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_NIGHTLY_VERSION" \
    --form "value=$new_nightly_ver"

curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_PATCH_RELEASE_VERSION" \
    --form "value=$new_patch_ver"

curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_MINOR_RELEASE_VERSION" \
    --form "value=$new_minor_ver"

curl --request PUT -H "PRIVATE-TOKEN: $(cat $CI_CUSTOM_JOB_TOKEN)" \
    "$HESTIA_API_URL/variables/CI_MAJOR_RELEASE_VERSION" \
    --form "value=$new_major_ver"
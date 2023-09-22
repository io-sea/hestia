#!/usr/bin/env sh
ARCH=x86_64

# Upload packages to registry
registry_url="$HESTIA_API_URL/packages/generic/hestia/$HESTIA_PROJECT_VERSION"

main_rpm=hestia-$HESTIA_PROJECT_VERSION-1.$ARCH.rpm
devel_rpm=hestia-devel-$HESTIA_PROJECT_VERSION-1.$ARCH.rpm
debuginfo_rpm=hestia-debuginfo-$HESTIA_PROJECT_VERSION-1.$ARCH.rpm
src_rpm=hestia-$HESTIA_PROJECT_VERSION-1.src.rpm

main_tar=hestia-$HESTIA_PROJECT_VERSION-Linux.tar.gz
docs_tar=hestia-docs-$HESTIA_PROJECT_VERSION.tar.gz

# RPMs: main, devel, tests, source
while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url/$main_rpm" \
    --upload-file "$CMAKE_BUILD_DIR/$main_rpm"
do echo "will retry in 2 seconds"; sleep 2; done

while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url/$devel_rpm" \
    --upload-file "$CMAKE_BUILD_DIR/$devel_rpm"
do echo "will retry in 2 seconds"; sleep 2; done

while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url/$debuginfo_rpm" \
    --upload-file "$CMAKE_BUILD_DIR/$debuginfo_rpm"
do echo "will retry in 2 seconds"; sleep 2; done

while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url/$src_rpm" \
    --upload-file "$CMAKE_BUILD_DIR/$src_rpm"
do echo "will retry in 2 seconds"; sleep 2; done

# TARs: main, source, docs
while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url/$main_tar" \
    --upload-file "$CMAKE_BUILD_DIR/$main_tar"
do echo "will retry in 2 seconds"; sleep 2; done

while ! curl --request PUT -H "JOB-TOKEN: $CI_JOB_TOKEN" \
    "$registry_url/$docs_tar" \
    --upload-file "$CMAKE_BUILD_DIR/$docs_tar"
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
            \"url\": \"$registry_url/$main_rpm\",
            \"direct_asset_path\": \"/binaries/$main_rpm\",
            \"link_type\": \"package\"
          },
          {
            \"name\": \"Hestia Development Headers RPM\",
            \"url\": \"$registry_url/$devel_rpm\",
            \"direct_asset_path\": \"/binaries/$devel_rpm\",
            \"link_type\": \"package\"
          },
          {
            \"name\": \"Hestia Debug Information RPM\",
            \"url\": \"$registry_url/$debuginfo_rpm\",
            \"direct_asset_path\": \"/binaries/$debuginfo_rpm\",
            \"link_type\": \"package\"
          },
          {
            \"name\": \"Hestia Executable tarball\",
            \"url\": \"$registry_url/$main_tar\",
            \"direct_asset_path\": \"/binaries/$main_tar\",
            \"link_type\": \"package\"
          },
          {
            \"name\": \"Hestia Documentation Tarball\",
            \"url\": \"$registry_url/$docs_tar\",
            \"direct_asset_path\": \"/binaries/$docs_tar\",
            \"link_type\": \"package\"
          }
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
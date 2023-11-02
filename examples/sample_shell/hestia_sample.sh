# dnf install -y curl jq mlocate
# updatedb
# HESTIA_CONFIG=$(locate hestia_controller_with_backends.yaml)
# hestia server --config $HESTIA_CONFIG&
# jobs

export HESTIA_ENDPOINT=127.0.0.1:8080

echo "File for upload" > my_file.dat

# Get the user token - we have one default user, who in turn has one token in 
# their tokens array
curl $HESTIA_ENDPOINT/api/v1/users > tmp.json
HESTIA_TOKEN=$(cat tmp.json | jq  -r '.[0].tokens[0].value')
rm tmp.json

# Create the object - grab its id from the returned object array
curl -X PUT -H "authorization: ${HESTIA_TOKEN}" $HESTIA_ENDPOINT/api/v1/objects > tmp.json
HESTIA_OBJECT_ID=$(cat tmp.json | jq  -r '.[0].id')
rm tmp.json

# Upload a file to the server - note the '@' is needed to tell curl to look 
# for binary data in this path.
curl -X PUT --data-binary "@my_file.dat" \
  -H "hestia.hsm_action.action: put_data" \
  -H "hestia.hsm_action.subject: object" \
  -H "hestia.hsm_action.subject_key: ${HESTIA_OBJECT_ID}" \
  -H "authorization: ${HESTIA_TOKEN}" \
  $HESTIA_ENDPOINT/api/v1/actions/ > tmp.json

# Check the status - note in this hestia version it always shows 'running'
# fixed in later versions.
HESTIA_STATUS=$(cat tmp.json | jq  -r '.status')
echo $HESTIA_STATUS

# Get the file back
curl \
  -H "hestia.hsm_action.action: get_data" \
  -H "hestia.hsm_action.subject: object" \
  -H "hestia.hsm_action.subject_key: ${HESTIA_OBJECT_ID}" \
  -H "authorization: ${HESTIA_TOKEN}" \
  $HESTIA_ENDPOINT/api/v1/actions/ > my_file_returned.dat

HESTIA_STATUS=$(cat tmp.json | jq  -r '.status')
echo $HESTIA_STATUS

# Release the data on a tier
curl \
  -H "hestia.hsm_action.action: release_data" \
  -H "hestia.hsm_action.subject: object" \
  -H "hestia.hsm_action.source_tier: 0" \
  -H "hestia.hsm_action.subject_key: ${HESTIA_OBJECT_ID}" \
  -H "authorization: ${HESTIA_TOKEN}" \
  $HESTIA_ENDPOINT/api/v1/actions/

# Delete the object overall
curl -X DELETE -H "authorization: ${HESTIA_TOKEN}" $HESTIA_ENDPOINT/api/v1/objects/$HESTIA_OBJECT_ID



# jdmt

## Requirement

.env File with the following entries:
 - accessKeyId
 - secretAccessKey
 - region
 - gkey --> Google-Maps-API-Key
 - endpoint
 - query url
 - data url

 ## Deployment

builds the scripts in src folder and uploads it to jdmt.develop.
```npm run upload```

AWS CLI commands to package, deploy and describe outputs defined within the cloudformation stack:

```bash
sam package --template-file lambda/template.yml  --output-template-file packaged.yaml  --s3-bucket jdmt-deployment
sam deploy --template-file packaged.yaml  --stack-name jdmt  --capabilities CAPABILITY_IAM 
```

## Local testing

Run the following command from parent folder to test function in local Docker container.
sample.json may contain senor data to test varous scenarios.

```bash 
sam local start-api

or 

sam local invoke "getAllDevices" -e /tests/sample.json 
```
const s3 = require('s3');
require('dotenv').config();

const { accessKeyId, secretAccessKey, region } = process.env;
const client = s3.createClient({ s3Options: { accessKeyId, secretAccessKey, region } });
const params = { localDir: 'build', s3Params: { Bucket: 'jdmt-static-web' } };

client.uploadDir(params);

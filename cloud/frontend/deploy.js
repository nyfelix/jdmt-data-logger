const s3 = require('s3');
require('dotenv').config();

const client = s3.createClient({ s3Options: { ...process.env } });
const params = { localDir: './build', s3Params: { Bucket: 'jdmt-static-web' } };

client.uploadDir(params);

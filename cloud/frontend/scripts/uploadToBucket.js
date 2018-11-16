const s3 = require('s3');
const dotenv = require('dotenv');

dotenv.config();

const client = s3.createClient({
  s3Options: {
    accessKeyId: process.env.accessKeyId,
    secretAccessKey: process.env.secretAccessKey,
    region: process.env.region,
  },
});

const params = {
  localDir: './build',
  s3Params: {
    Bucket: 'jdmt-static-web',
  },
};

client.uploadDir(params);

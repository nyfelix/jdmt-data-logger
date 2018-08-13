import s3 from 's3';
import dotenv from 'dotenv';

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
    Bucket: 'develop.jdmt',
  },
};

client.uploadDir(params);

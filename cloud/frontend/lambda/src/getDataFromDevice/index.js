const AWS = require('aws-sdk');

const ddb = new AWS.DynamoDB.DocumentClient();
const TableName = 'jdmt-device-data';

const getChunks = (data, resolution) => new Array(Math.ceil(data.length / resolution))
  .fill()
  .map((_, i) => (data.slice(i * resolution, i * resolution + resolution)));

const compressData = (data, resolution, attribute) => (
  getChunks(data, resolution).map((chunk) => {
    const max = Math.max(...chunk.map(e => e[attribute]));
    const min = Math.min(...chunk.map(e => e[attribute]));
    const average = chunk.reduce((a, b) => (a + b[attribute]), 0) / chunk.length;
    const from = chunk[0].time;
    const to = chunk[chunk.length - 1].time;

    return {
      max, min, average, from, to,
    };
  }));

exports.handler = async (event) => {
  const {
    id, attributes, from, to, resolution,
  } = event.queryStringParameters;
  const defaultFrom = new Date(new Date().setMonth(new Date().getMonth() - 3)).toISOString();
  const defaultTo = new Date().toISOString();
  const ProjectionExpression = attributes ? (`${attributes},#time`) : null;
  const data = await ddb.query({
    TableName,
    ProjectionExpression,
    KeyConditionExpression: 'dev_id = :id and #time BETWEEN :from AND :to',
    ExpressionAttributeNames: { '#time': 'time' },
    ExpressionAttributeValues: { ':id': id, ':from': from || defaultFrom, ':to': to || defaultTo },
  }).promise();

  if (resolution && data.Items && data.Items.length !== 0) {
    const sortedData = data.Items.sort((a, b) => (a.time - b.time));
    const collectedData = Object.keys(data.Items[0]).filter(x => x !== 'time' && x !== 'dev_id')
      .map(attribute => ({ attribute, values: compressData(sortedData, resolution, attribute) }));

    return {
      isBase64Encoded: false,
      headers: { 'Access-Control-Allow-Origin': '*' },
      body: JSON.stringify(collectedData),
    };
  }

  return {
    isBase64Encoded: false,
    headers: { 'Access-Control-Allow-Origin': '*' },
    body: JSON.stringify(data.Items),
  };
};

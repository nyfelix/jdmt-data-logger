
const AWS = require('aws-sdk');

const iot = new AWS.Iot();
const iotdata = new AWS.IotData({ endpoint: 'a2zzdzxcy5vwv0-ats.iot.eu-west-1.amazonaws.com' });

exports.handler = async () => new Promise((resolve, reject) => {
  try {
    iot.listThings({}, async (_, { things }) => {
      const devices = things
      // eslint-disable-next-line camelcase
        .map(({ thingName, attributes: { dev_id } }) => ({ name: thingName, id: dev_id }));
      const data = await Promise.all(devices.map(device => new Promise((res, rej) => {
        iotdata.getThingShadow({ thingName: device.name }, (error, response) => {
          if (error) {
            rej(error);
          } else {
            const { state: { reported } } = JSON.parse(response.payload);
            res({
              ...device, ...reported,
            });
          }
        });
      })));
      const response = {
        isBase64Encoded: false,
        body: JSON.stringify(data),
        headers: { 'Access-Control-Allow-Origin': '*' },
      };
      resolve(response);
    });
  } catch (error) {
    reject(error);
  }
});


const AWS = require('aws-sdk');

const iot = new AWS.Iot();
const iotdata = new AWS.IotData({ endpoint: 'a2zzdzxcy5vwv0-ats.iot.eu-west-1.amazonaws.com' });

exports.handler = async () => new Promise((resolve, reject) => {
  try {
    iot.listThings({}, (_, data) => {
      console.log(data);
      Promise.all(data.things.map(device => new Promise((res, rej) => {
        iotdata.getThingShadow({ thingName: device.thingName }, (error, response) => {
          if (error || response.payload.state === undefined) {
            rej(error);
            return;
          }
          const { lat, lng } = response.payload.state.reported;
          res({
            name: device.thingName, dev_id: device.attributes.dev_id, lat, lng,
          });
        });
      }))).then((devices) => {
        console.log(devices);
        const response = {
          isBase64Encoded: false,
          body: JSON.stringify(devices),
          headers: { 'Access-Control-Allow-Origin': '*' },
        };
        resolve(response);
      });
    });
  } catch (error) {
    reject(error);
  }
});

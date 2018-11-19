const AWS = require('aws-sdk');

const iot = new AWS.Iot();
const iotdata = new AWS.IotData({endpoint: 'a2zzdzxcy5vwv0-ats.iot.eu-west-1.amazonaws.com'});

exports.handler = () => new Promise((resolve, reject) => {
 // try {
      console.log("Start Request Hallo");

      iotdata.getThingShadow({thingName:'jmdt1'}, (error, response) => {
        console.log(error);
        console.log(response); 
        resolve(response);
      });
      
      /*const params = {
        topic: 'test/jmdt1',
        payload: JSON.stringify({
          "message": "Hello from Lambda"
        })
      }
      iotdata.publish(params, (err, res) => {
        if (err) return context.fail(err);
        console.log(res);
      });*/
      //
    /*iot.listThings({}, (_, data) => {
      console.log(data);
      const devices = data.things.map(device => ({name: device.thingName, ...device.attributes}));
      Promise.all(devices.map(async device => new Promise((res, rej) => {
        console.log(device);
        iotdata.getThingShadow({thingName:'jmdt1'}), (error, response) => {
          console.log(response);
          if(error){
            rej(error);
          }else{
            res({...device, ...response});  
          }
        };
      }))).then(body => {
        
        const response = {
          isBase64Encoded: false,
          statusCode: '200',
          body: JSON.stringify(body),
          headers: {
            'Access-Control-Allow-Origin': '*',
            },
          };
      resolve(response);
      });
      });
    */
    
      
  /*} catch (error) {
    reject({
      isBase64Encoded: false,
      statusCode: '401',
      headers: {
        'Access-Control-Allow-Origin': '*',
      },
    });
  }*/
});

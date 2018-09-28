import { GET_ALL_DEVICES, GET_DEVICE_DATA } from './actionTypes';

const itemKey = 'dev_id';
const time = 'time';

const initialState = {
  devices: [],
  deviceData: {},
  sensorHeaders: [],
};
export default(oldState = initialState, action) => {
  switch (action.type) {
    case GET_ALL_DEVICES: {
      const { devices } = action;
      return { ...oldState, devices };
    }
    case GET_DEVICE_DATA: {
      const { deviceData } = action;
      const sensorHeaders = Object.keys(deviceData[0]).filter(key => key !== time && key !== itemKey);
      const data = sensorHeaders.reduce((target, current) => {
        target[current] = deviceData.filter(item => item[current]).map(item => ({ y: item[current], x: new Date(item[time]) }));
        return target;
      }, {});


      return { ...oldState, deviceData: data, sensorHeaders };
    }
    default:
      return oldState;
  }
};

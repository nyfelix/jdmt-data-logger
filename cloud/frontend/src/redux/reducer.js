import { GET_ALL_DEVICES, GET_DEVICE_DATA } from './actionTypes';


const initialState = {
  devices: [],
};
export default(oldState = initialState, action) => {
  switch (action.type) {
    case GET_ALL_DEVICES: {
      const { devices } = action;
      const filteredDevices = devices.filter(x => x.id !== undefined);
      return { ...oldState, devices: filteredDevices };
    }
    case GET_DEVICE_DATA: {
      const { deviceData } = action;

      return { ...oldState, deviceData };
    }
    default:
      return oldState;
  }
};

import { GET_ALL_DEVICES, GET_DEVICE_DATA, FROM_CHANGE } from './actionTypes';

const initialState = {
  devices: [],
  from: null,
  resolution: 50,
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
    case FROM_CHANGE: {
      const { from } = action;

      return { ...oldState, from };
    }
    default:
      return oldState;
  }
};

import { GET_ALL_DEVICES, GET_DEVICE_DATA, ON_CHANGE } from './actionTypes';


const initialState = {
  devices: [],
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
    case ON_CHANGE: {
      const { id, value } = action;

      return { ...oldState, [id]: value };
    }
    default:
      return oldState;
  }
};

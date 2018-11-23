const initialState = {
  devices: [],
  from: null,
  resolution: 50,
};
export default(oldState = initialState, action) => {
  switch (action.type) {
    case 'GET_ALL_DEVICES': {
      const { devices } = action;
      console.log(devices);
      return { ...oldState, devices };
    }
    case 'GET_DEVICE_DATA': {
      const { deviceData } = action;

      return { ...oldState, deviceData };
    }
    case 'FROM_CHANGE': {
      const { from } = action;

      return { ...oldState, from };
    }
    default:
      return oldState;
  }
};

const initialState = {
  devices: [],
  from: null,
  resolution: 50,
};
export default(oldState = initialState, action) => {
  const { type, ...rest } = action;
  if (type === 'GET_ALL_DEVICES' || type === 'GET_DEVICE_DATA' || type === 'FROM_CHANGE') {
    return { ...oldState, ...rest };
  }
  return oldState;
};

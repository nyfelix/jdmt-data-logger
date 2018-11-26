import 'whatwg-fetch';

const fetchJson = async (url) => {
  const response = await fetch(`${process.env.NODE_ENV === 'DEBUG' ? process.env.local : process.env.prod}/${url}`);
  return response.json();
};

export const getDevices = () => async (dispatch) => {
  const devices = await fetchJson('device/query');
  dispatch({ type: 'GET_ALL_DEVICES', devices });
};

export const getDeviceData = ({ deviceId, from, resolution }) => async (dispatch) => {
  const fromQuery = from ? `&from=${from}` : '';
  const resolutionQuery = resolution ? `&resolution=${resolution}` : '';
  const url = `device/data?id=${deviceId}${resolutionQuery}${fromQuery}`;
  const deviceData = await fetchJson(url);
  dispatch({ type: 'GET_DEVICE_DATA', deviceData });
};

export const fromChange = (from, query) => (dispatch) => {
  dispatch({ type: 'FROM_CHANGE', from });
  dispatch(getDeviceData({ ...query, from }));
};

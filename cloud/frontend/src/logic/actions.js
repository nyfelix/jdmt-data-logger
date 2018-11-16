import { GET_ALL_DEVICES, GET_DEVICE_DATA, FROM_CHANGE } from './actionTypes';
import 'whatwg-fetch';

const fetchJson = async (url) => {
  const response = await fetch(url);
  return response.json();
};

export const getDevices = () => async (dispatch) => {
  const devices = await fetchJson('https://2iuv5z9qnk.execute-api.eu-west-1.amazonaws.com/prod/device/query');
  dispatch({ type: GET_ALL_DEVICES, devices });
};

export const getDeviceData = ({ deviceId, from, resolution }) => async (dispatch) => {
  const fromQuery = from ? `&from=${from}` : '';
  const resolutionQuery = resolution ? `&resolution=${resolution}` : '';
  const url = `https://2iuv5z9qnk.execute-api.eu-west-1.amazonaws.com/prod/device/data?id=${deviceId}${resolutionQuery}${fromQuery}`;
  const deviceData = await fetchJson(url);
  dispatch({ type: GET_DEVICE_DATA, deviceData });
};

export const fromChange = (from, query) => (dispatch) => {
  dispatch({ type: FROM_CHANGE, from });
  dispatch(getDeviceData({ ...query, from }));
};

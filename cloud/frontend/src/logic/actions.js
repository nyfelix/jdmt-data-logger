import { GET_ALL_DEVICES, GET_DEVICE_DATA, ON_CHANGE } from './actionTypes';
import 'whatwg-fetch';

export const getDevices = () => (dispatch) => {
  fetch('https://2iuv5z9qnk.execute-api.eu-west-1.amazonaws.com/prod/device/query')
    .then(response => response.json())
    .then((devices) => {
      dispatch({
        type: GET_ALL_DEVICES,
        devices,
      });
    });
};
export const getDeviceData = (deviceId, from, resolution) => (dispatch) => {
  const fromQuery = from ? `&from=${from}` : '';
  const resolutionQuery = resolution ? `&resolution=${resolution}` : '';
  fetch(`https://2iuv5z9qnk.execute-api.eu-west-1.amazonaws.com/prod/device/data?id=${deviceId}${resolutionQuery}${fromQuery}`)
    .then(response => response.json())
    .then((deviceData) => {
      dispatch({
        type: GET_DEVICE_DATA,
        deviceData,
      });
    });
};

export const onChange = (id, value) => dispatch => dispatch({
  type: ON_CHANGE,
  value,
  id,
});

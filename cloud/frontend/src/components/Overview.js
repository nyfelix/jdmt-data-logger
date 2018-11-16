import React from 'react';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import * as Actions from '../logic/actions';
import LineContainer from './LineContainer';

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(
  ({ deviceData }) => (
    deviceData ? deviceData.map(device => <LineContainer key={device.attribute} {...device} />)
      : null),
);

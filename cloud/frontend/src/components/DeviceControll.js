import React from 'react';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import * as Actions from '../logic/actions';

const dateNowMinusDays = days => new Date(new Date().setDate(new Date().getDate() - days))
  .toISOString();

const DeviceControll = ({ onChange }) => (
  <div className="form-group col-md-12">
    Zeitspanne
    <select className="form-control" onChange={({ target: { value } }) => onChange('from', value)}>
      <option value={dateNowMinusDays(4)}>
    Letzten 4 Tage
      </option>
      <option value={dateNowMinusDays(31)}>
    Letzten Monat
      </option>
      <option value={dateNowMinusDays(365)}>
    Letztes Jahr
      </option>
    </select>
  </div>
);

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(DeviceControll);

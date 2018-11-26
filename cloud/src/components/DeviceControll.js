import React from 'react';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';
import * as Actions from '../logic/actions';
import dateNowMinusDays from '../logic/dateNowMinusDays';

export default withRouter(connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(({
  fromChange, resolution, match: { params: { deviceId } },
}) => (
  <div className="form-group col-md-12">
    Zeitspanne
    <select className="form-control" onChange={({ target: { value } }) => fromChange(value, { resolution, deviceId })}>
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
)));

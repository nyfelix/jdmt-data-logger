import React from 'react';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import DayPicker, { DateUtils } from 'react-day-picker';
import * as Actions from './redux/actions';
import 'react-day-picker/lib/style.css';

const DeviceControll = () => (


  <div className="form-group col-md-4">
    <label htmlFor="inputState">
Zeitspanne
    </label>
    <select id="inputState" className="form-control">
      <option selected>
Letzten 4 Tage
      </option>
      <option>
Letzten Monat
      </option>
      <option>
Letztes Jahr
      </option>
    </select>
  </div>

);

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(DeviceControll);

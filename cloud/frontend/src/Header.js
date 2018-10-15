import React from 'react';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import Sliders from 'react-feather/dist/icons/sliders';
import Heart from 'react-feather/dist/icons/heart';
import * as Actions from './redux/actions';

const header = props => (
  <nav className="sidebar sidebar-sticky" key="headernav">
    <div className="sidebar-content">
      <a className="sidebar-brand" href="index.html">
        <i className="align-middle" data-feather="box" />
        <span className="align-middle">
JDMT
        </span>
      </a>
      <ul className="sidebar-nav">
        <li className="sidebar-header">
Menu
        </li>
        <li className="sidebar-item active">
          <a href="index.html" className="sidebar-link">
            <Sliders />
            <span className="align-middle">
Dashboard
            </span>
          </a>
        </li>
        {props.devices.map(device => (
          <li key={device.id} className="sidebar-item active">
            <a href="index.html" className="sidebar-link">
              <Heart />
              <span className="align-middle">
                {device.id}
              </span>
            </a>
          </li>
        ))}

      </ul>
    </div>
  </nav>
);

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(header);

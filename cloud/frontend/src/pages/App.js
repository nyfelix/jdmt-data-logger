import React from 'react';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';

import { withRouter } from 'react-router';
import * as Actions from '../logic/actions';
import Header from '../components/Header';

class Content extends React.Component {
  componentDidMount() {
    const {
      getDeviceData, from, resolution, match: { params: { deviceId } },
    } = this.props;
    getDeviceData({ from, resolution, deviceId });
  }

  render() {
    const { children, match: { params: { deviceId } } } = this.props;

    return (
      <div className="wrapper">
        <div className="d-flex">
          <Header />
          <div className="main">
            <div className="main" key="headertoggle">
              <nav className="navbar navbar-expand navbar-light bg-white">
                <span className="sidebar-toggle d-flex mr-2">
                  <i className="hamburger align-self-center" />
                </span>
              </nav>
            </div>
            <main className="content">
              <div className="container-fluid p-0">
                <div className="clearfix">
                  <h1 className="h3 mb-3">
                    {deviceId || 'Dashboard'}
                  </h1>
                </div>
                {children}
              </div>
            </main>
          </div>
        </div>
      </div>
    );
  }
}

export default withRouter(connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(Content));

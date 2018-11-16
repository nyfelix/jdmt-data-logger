import React from 'react';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import { withRouter } from 'react-router';

import * as Actions from '../logic/actions';
import LineChart from '../components/LineChart';
import DeviceControll from '../components/DeviceControll';
import Map from '../components/Map';

class DetailView extends React.Component {
  componentDidMount() {
    const {
      getDeviceData, from, resolution, match: { params: { deviceId } },
    } = this.props;
    getDeviceData({ from, resolution, deviceId });
  }

  render() {
    const { deviceData } = this.props;

    return (
      <div>
        <div className="form-row">
          <div className="col-3 col-md-3 col-lg-3 d-flex">
            <div className="card flex-fill w-100">
              <div className="card-body">
                <DeviceControll />
              </div>
            </div>
          </div>
          <div className="col-9 col-lg-9 d-flex">
            <div className="card flex-fill w-100">
              <div className="card-body">
                <Map />
              </div>
            </div>
          </div>
        </div>
        <div className="row">
          {deviceData && deviceData.map(({ attribute, values }) => (
            <div key={attribute} className="col-6 col-md-6 col-xl-6 col-lg-6 d-flex order-1 order-xl-2">
              <div className="card flex-fill w-100">
                <div className="card-header">
                  <h5 className="card-title mb-0">
                    {attribute}
                  </h5>
                </div>
                <div className="card-body d-flex">
                  <div className="align-self-center w-100">
                    <div className="py-3">
                      <LineChart {...{ values }} />
                    </div>
                  </div>
                </div>
              </div>
            </div>
          ))}
        </div>
      </div>
    );
  }
}

export default withRouter(connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(DetailView));

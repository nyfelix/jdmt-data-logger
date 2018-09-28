import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import React from 'react';
import { Line } from 'react-chartjs-2';
import * as Actions from './redux/actions';
import Map from './Map';
import logo from './logo.png';
import './App.css';

class App extends React.Component {
  constructor(props) {
    super(props);
    props.getDevices();
  }

  render() {
    const {
      devices, getDeviceData, sensorHeaders, deviceData,
    } = this.props;
    if (deviceData.humidity !== null) { console.log(deviceData.humidity); }
    const label = sensorHeaders[0];

    return (
      <div className="App">
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <h1 className="App-title">
          JDMT
          </h1>
        </header>

        <div className="container">
          <Map devices={devices} onMarkerClick={event => getDeviceData(event)} />
          {
          deviceData.humidity && deviceData.humidity.length !== 0 && (
          <Line
            data={{
              datasets: [
                {
                  label,
                  data: deviceData.humidity,
                },
              ],

            }}
            options={{
              yAxisID: 'time',
              xAxisID: 'label',
            }}
          />
          )}
        </div>
      </div>
    );
  }
}

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(App);

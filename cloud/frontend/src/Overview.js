import React from 'react';
import { Line } from 'react-chartjs-2';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import * as Actions from './redux/actions';

const entry = measurements => (
  <div className="col-12 col-md-12 col-xl-6 d-flex order-1 order-xl-2">
    <div className="card flex-fill w-100">
      <div className="card-header">
        <h5 className="card-title mb-0">
          {measurements.attribute}
          {console.log(measurements)}
        </h5>
      </div>
      <div className="card-body d-flex">
        <div className="align-self-center w-100">
          <div className="py-3">
            <Line
              data={{
                datasets: [
                  {
                    backgroundColor: 'rgba(75,192,192,0.4)',
                    borderColor: 'rgba(75,192,192,1)',
                    borderCapStyle: 'butt',
                    borderDashOffset: 0.0,
                    borderJoinStyle: 'miter',
                    pointBorderColor: 'rgba(75,192,192,1)',
                    pointBackgroundColor: '#fff',
                    pointBorderWidth: 1,
                    pointHoverRadius: 5,
                    pointHoverBackgroundColor: 'rgba(75,192,192,1)',
                    pointHoverBorderColor: 'rgba(220,220,220,1)',
                    pointHoverBorderWidth: 2,
                    pointRadius: 1,
                    pointHitRadius: 10,
                    data: measurements.values.map(item => ({ y: item.average, x: new Date(item.from) })),
                  },
                ],
              }}
              legend={{
                display: false,
              }}
              options={{
                scales: {
                  xAxes: [{
                    type: 'time',

                  }],
                },
              }}
            />
          </div>
        </div>
      </div>
    </div>
  </div>
);
const Overview = ({ deviceData }) => (
  <div className="row">
    {deviceData && deviceData.map(entry)}
  </div>
);

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(Overview);

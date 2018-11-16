import React from 'react';
import { Line } from 'react-chartjs-2';

const dataset = [
  {
    label: 'average',
    colorFull: 'rgba(126,198,121,1)',
    colorTransparent: 'rgba(126,198,121,0.6)',
  },
  {
    label: 'max',
    colorFull: 'rgba(129,150,143,1)',
    colorTransparent: 'rgba(129,150,143,0.6)',
  },
  {
    label: 'min',
    colorFull: 'rgba(206,134,189,1)',
    colorTransparent: 'rgba(206,134,189,0.6)',
  },
];

const createDataset = (options, data) => ({
  borderColor: options.colorTransparent,
  backgroundColor: options.colorTransparent,
  fill: false,
  pointBorderColor: options.colorFull,
  pointBackgroundColor: options.colorFull,
  pointBorderWidth: 1,
  pointHoverRadius: 5,
  pointHoverBackgroundColor: options.colorFull,
  pointHoverBorderColor: 'rgba(220,220,220,1)',
  pointHoverBorderWidth: 2,
  pointRadius: 5,
  pointHitRadius: 10,
  data: data.map(item => ({ y: item[options.label], x: new Date(item.from) })),
  label: options.label,
});

export default ({ attribute, values }) => (
  <div className="col-6 col-md-6 col-xl-12 col-lg-12 d-flex order-1 order-xl-2">
    <div className="card flex-fill w-100">
      <div className="card-header">
        <h5 className="card-title mb-0">
          {attribute}
        </h5>
      </div>
      <div className="card-body d-flex">
        <div className="align-self-center w-100">
          <div className="py-3">
            <Line
              data={{
                datasets: dataset.map(options => createDataset(options, values)),
              }}
              options={{
                scales: {
                  xAxes: [
                    {
                      type: 'time',
                    },
                  ],
                },
              }}
            />
          </div>
        </div>
      </div>
    </div>
  </div>
);

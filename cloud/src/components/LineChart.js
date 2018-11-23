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

export default ({ values }) => (
  <Line
    data={{
      datasets: dataset.map(options => ({
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
        data: values.map(item => ({ y: item[options.label], x: new Date(item.from) })),
        label: options.label,
      })),
    }}
    options={{ scales: { xAxes: [{ type: 'time' }] } }}
  />
);

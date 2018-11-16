import React from 'react';
import LineChart from './LineChart';

export default ({ attribute, values }) => (
  <div className="col-6 col-md-6 col-xl-6 col-lg-6 d-flex order-1 order-xl-2">
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
);

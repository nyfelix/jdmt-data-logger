import React from 'react';
import { withRouter } from 'react-router';

export default withRouter(({ children, match: { params: { deviceId } } }) => (
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
));

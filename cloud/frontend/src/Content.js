import React from 'react';

export default props => (
  <div className="main">
    <div className="main" key="headertoggle">
      <nav className="navbar navbar-expand navbar-light bg-white">
        <a className="sidebar-toggle d-flex mr-2">
          <i className="hamburger align-self-center" />
        </a>
      </nav>
    </div>

    <main className="content">
      <div className="container-fluid p-0">
        <div className="clearfix">
          <h1 className="h3 mb-3">
            {props.header}
          </h1>
        </div>
        {props.children}
      </div>
    </main>
  </div>);

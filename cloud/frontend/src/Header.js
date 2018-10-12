import React from 'react';
import Sliders from 'react-feather/dist/icons/sliders';

export default () => (
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

      </ul>
    </div>
  </nav>
);

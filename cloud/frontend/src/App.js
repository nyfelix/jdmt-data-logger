import React from 'react';
import Map from './Map';
import logo from './logo.svg';
import './App.css';

export default () => (
  <div className="App">
    <header className="App-header">
      <img src={logo} className="App-logo" alt="logo" />
      <h1 className="App-title">
          Welcome to React. :)
      </h1>
    </header>

    <div className="container">
      <Map markers={[{ lat: -34.397, lng: 150.644 }, { lat: -34.297, lng: 150.644 }]} />
    </div>
  </div>
);

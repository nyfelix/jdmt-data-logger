import React from 'react';
import {
  BrowserRouter, Route, Switch,
} from 'react-router-dom';

import Map from './Map';
import Overview from './Overview';
import Header from './Header';
import Content from './Content';

export default () => (
  <BrowserRouter>
    <div className="wrapper">
      <div className="d-flex">
        <Header />
        <Switch>
          <Route
            exact
            path="/"
            component={() => (
              <Content header="Dashboard">
                <Map />
                <Overview />
              </Content>)}
          />
        </Switch>
      </div>
    </div>
  </BrowserRouter>
);

import React from 'react';
import { BrowserRouter, Route, Switch } from 'react-router-dom';

import Map from './components/Map';
import Overview from './components/Overview';
import Header from './components/Header';
import Content from './components/Content';
import DeviceControll from './components/DeviceControll';

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
              <Content>
                <div className="row">
                  <div className="col-12 col-lg-12 d-flex">
                    <div className="card flex-fill w-100">
                      <div className="card-body">
                        <Map />
                      </div>
                    </div>
                  </div>
                </div>
                <div className="row">
                  <Overview />
                </div>
              </Content>
            )}
          />
          <Route
            exact
            path="/:deviceId"
            component={() => (
              <Content>
                <div className="form-row">
                  <div className="col-3 col-md-3 col-lg-3 d-flex">
                    <div className="card flex-fill w-100">
                      <div className="card-body">
                        <DeviceControll />
                      </div>
                    </div>
                  </div>
                  <div className="col-9 col-lg-9 d-flex">
                    <div className="card flex-fill w-100">
                      <div className="card-body">
                        <Map />
                      </div>
                    </div>
                  </div>
                </div>
                <div className="row">
                  <Overview />
                </div>
              </Content>
            )}
          />
        </Switch>
      </div>
    </div>
  </BrowserRouter>
);

import React from 'react';
import { BrowserRouter, Route, Switch } from 'react-router-dom';

import Overview from './pages/Overview';
import DetailView from './pages/DetailView';
import App from './pages/App';

export default () => (
  <BrowserRouter>
    <App>
      <Switch>
        <Route exact path="/" component={() => (<Overview />)} />
        <Route exact path="/:deviceId" component={() => (<DetailView />)} />
      </Switch>
    </App>
  </BrowserRouter>
);

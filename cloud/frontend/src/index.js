import React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'react-redux';

import Map from './Map';
import Overview from './Overview';
import Header from './Header';
import Content from './Content';
import registerServiceWorker from './registerServiceWorker';
import store from './store';

ReactDOM.render(
  <Provider store={store}>
    <div className="wrapper">
      <div className="d-flex">
        <Header />
        <Content header="Dashboard">
          <Map />
          <Overview />
        </Content>
      </div>
    </div>
  </Provider>,
  document.getElementById('root'),
);
registerServiceWorker();

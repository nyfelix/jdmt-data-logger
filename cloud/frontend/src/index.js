import React from 'react';
import ReactDOM from 'react-dom';
import { createBrowserHistory, createMemoryHistory } from 'history';
import { Provider } from 'react-redux';

import 'babel-polyfill';

import registerServiceWorker from './registerServiceWorker';
import store from './store';
import Routes from './routes';

require('dotenv').config();


const history = typeof window !== 'undefined'
  ? createBrowserHistory()
  : createMemoryHistory();

ReactDOM.render(
  <Provider store={store}>
    <Routes history={history} />
  </Provider>,
  document.getElementById('root'),
);
registerServiceWorker();

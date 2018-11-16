import {
  createStore, applyMiddleware, combineReducers, compose,
} from 'redux';
import thunk from 'redux-thunk';
import { routerMiddleware } from 'react-router-redux';
import createHistory from 'history/createBrowserHistory';

import reducer from './logic/reducer';

const history = createHistory();
const reduxRouterMiddleware = routerMiddleware(history);
const store = createStore(
  combineReducers({ reducer }),
  undefined,
  compose(applyMiddleware(thunk, reduxRouterMiddleware)),
);

export default store;

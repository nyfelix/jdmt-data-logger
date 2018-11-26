import {
  createStore, applyMiddleware, combineReducers, compose,
} from 'redux';
import thunk from 'redux-thunk';
import { routerMiddleware } from 'react-router-redux';
import createHistory from 'history/createBrowserHistory';

import reducer from './logic/reducer';

export default createStore(
  combineReducers({ reducer }),
  compose(applyMiddleware(thunk, routerMiddleware(createHistory()))),
);

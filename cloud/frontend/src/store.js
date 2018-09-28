import {
  createStore, applyMiddleware, combineReducers, compose,
} from 'redux';
import thunk from 'redux-thunk';
import { routerMiddleware } from 'react-router-redux';
import createHistory from 'history/createBrowserHistory';

import reducer from './redux/reducer';

const history = createHistory();
const reduxRouterMiddleware = routerMiddleware(history);
const composeEnhancers = window.__REDUX_DEVTOOLS_EXTENSION_COMPOSE__ || compose;
const store = createStore(
  combineReducers({ reducer }),
  undefined,
  composeEnhancers(applyMiddleware(thunk, reduxRouterMiddleware)),
);

export default store;

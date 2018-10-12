import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import React from 'react';
import {
  withScriptjs,
  withGoogleMap,
  GoogleMap,
  Marker,
} from 'react-google-maps';
import * as Actions from './redux/actions';

const MapWithAMarker = withScriptjs(withGoogleMap((props) => {
  const { devices, onMarkerClick } = props;
  return (
    <GoogleMap
      defaultZoom={8}
      defaultCenter={{ lat: 47.533125, lng: 8.731556 }}
      defaultOptions={{
        styles: [{ disableDefaultUI: true, featureType: 'road', stylers: [{ visibility: 'off' }] }],
        disableDefaultUI: true,
      }}
    >
      {
        devices.map((device) => {
          const { lat, lng, id } = device;
          return !id ? '' : (<Marker key={id} id={id} onClick={() => onMarkerClick(id)} position={{ lat: +lat, lng: +lng }} />);
        })
      }
    </GoogleMap>
  );
}));


class Map extends React.Component {
  constructor(props) {
    super(props);
    props.getDevices();
  }

  render() {
    const {
      getDeviceData, devices,
    } = this.props;
    return (
      <div className="row">
        <div className="col-12 col-lg-12 d-flex">
          <div className="card flex-fill w-100">
            <div className="card-body">
              <MapWithAMarker
                googleMapURL="https://maps.googleapis.com/maps/api/js?key=AIzaSyBJd_9-gMHZCEXqWq4XrHN_QydPV-m_Q-w"
                loadingElement={<div style={{ height: '100%' }} />}
                containerElement={<div style={{ height: '400px' }} />}
                mapElement={<div style={{ height: '100%' }} />}
                devices={devices}
                onMarkerClick={event => getDeviceData(event)}
              />
            </div>
          </div>
        </div>
      </div>
    );
  }
}

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(Map);

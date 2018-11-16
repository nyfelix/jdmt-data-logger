/* global google */
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import React from 'react';
import {
  withScriptjs,
  withGoogleMap,
  GoogleMap,
  Marker,
} from 'react-google-maps';
import * as Actions from '../logic/actions';

const MapWithAMarker = withScriptjs(
  withGoogleMap((props) => {
    const { devices, onMarkerClick } = props;
    return (
      <GoogleMap
        defaultZoom={8}
        defaultCenter={{ lat: 47.533125, lng: 8.731556 }}
        defaultOptions={{
          styles: [
            {
              disableDefaultUI: true,
              featureType: 'road',
              stylers: [{ visibility: 'off' }],
            },
          ],
          disableDefaultUI: true,
        }}
      >
        {devices.map((device) => {
          const { lat, lng, id } = device;
          return !id ? (
            ''
          ) : (
            <Marker
              key={id}
              animation={google.maps.Animation.DROP}
              id={id}
              onClick={() => onMarkerClick(id)}
              position={{ lat: +lat, lng: +lng }}
            />
          );
        })}
      </GoogleMap>
    );
  }),
);

class Map extends React.Component {
  constructor(props) {
    super(props);
    props.getDevices();
  }

  render() {
    const {
      getDeviceData, devices, from, resolution,
    } = this.props;
    return (
      <MapWithAMarker
        googleMapURL="https://maps.googleapis.com/maps/api/js?key=AIzaSyBUSQcWODHeYXS5v1qpUH8JcnwWajebp7o"
        loadingElement={<div style={{ height: '100%' }} />}
        containerElement={<div style={{ height: '380px' }} />}
        mapElement={<div style={{ height: '100%' }} />}
        devices={devices}
        onMarkerClick={event => getDeviceData(event, from, resolution)}
      />
    );
  }
}

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(Map);

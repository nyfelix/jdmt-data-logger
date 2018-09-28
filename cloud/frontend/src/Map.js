import React from 'react';
import {
  withScriptjs,
  withGoogleMap,
  GoogleMap,
  Marker,
} from 'react-google-maps';

const MapWithAMarker = withScriptjs(withGoogleMap((props) => {
  const { devices, onMarkerClick } = props;
  return (
    <GoogleMap
      defaultZoom={8}
      defaultCenter={{ lat: 47.533125, lng: 8.731556 }}
      defaultOptions={{ streetViewControl: false }}
    >
      {devices.map((device) => {
        const { lat, lng, id } = device;
        if (!id) {
          return '';
        }
        return (
          <Marker
            key={id}
            id={id}
            onClick={() => onMarkerClick(id)}
            position={{ lat: +lat, lng: +lng }}
          />
        );
      })}
    </GoogleMap>
  );
}));

export default props => (
  <MapWithAMarker
    googleMapURL="https://maps.googleapis.com/maps/api/js?key=AIzaSyBJd_9-gMHZCEXqWq4XrHN_QydPV-m_Q-w"
    loadingElement={<div style={{ height: '100%' }} />}
    containerElement={<div style={{ height: '400px' }} />}
    mapElement={<div style={{ height: '100%' }} />}
    devices={props.devices}
    onMarkerClick={props.onMarkerClick}
  />
);

import React from 'react';
import {
  withScriptjs,
  withGoogleMap,
  GoogleMap,
  Marker,
} from 'react-google-maps';

const MapWithAMarker = withScriptjs(withGoogleMap((props) => {
  const { markers } = props;
  return (
    <GoogleMap
      defaultZoom={8}
      defaultCenter={{ lat: -34.397, lng: 150.644 }}
      defaultOptions={{ streetViewControl: false }}
    >
      {markers.map(marker => <Marker key={marker.lat + marker.lng} position={{ ...marker }} />)}
    </GoogleMap>
  );
}));

export default (props) => {
  const { markers } = props;
  return (
    <MapWithAMarker
      googleMapURL="https://maps.googleapis.com/maps/api/js?key=AIzaSyBJd_9-gMHZCEXqWq4XrHN_QydPV-m_Q-w"
      loadingElement={<div style={{ height: '100%' }} />}
      containerElement={<div style={{ height: '400px' }} />}
      mapElement={<div style={{ height: '100%' }} />}
      markers={markers}
    />
  );
};

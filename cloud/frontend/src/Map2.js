import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import React from 'react';
import { VectorMap } from 'react-jvectormap';
import * as Actions from './redux/actions';

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
      <div style={{ height: 500 }}>
        {devices && devices.length !== 0 && (
        <VectorMap
          map="ch_mill"
          backgroundColor="transparent"
          normalizeFunction="polynomial"
          hoverOpacity={0.7}
          containerStyle={{
            width: '100%',
            height: '100%',
          }}
          onMarkerClick={(_, index) => {
            getDeviceData(devices[+index].id);
          }}
          regionStyle={{
            initial: {
              fill: '#DCE3E8',
            },
          }}
          markerStyle={{
            initial: {
              r: 9,
              fill: '#007bff',
              'fill-opacity': 0.9,
              stroke: '#fff',
              'stroke-width': 7,
              'stroke-opacity': 0.4,
            },
            hover: {
              stroke: '#fff',
              'fill-opacity': 1,
              'stroke-width': 1.5,
            },
          }}
          markers={devices.map(x => ({ latLng: [+x.lat, +x.lng], name: x.id }))}
        />
        )}
      </div>

    );
  }
}

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(Map);

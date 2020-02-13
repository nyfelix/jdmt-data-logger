function Decoder(bytes, port) {
	// Decode an uplink message from a buffer
	// (array) of bytes to an object of fields.
	// temperature
	rawTemp = bytes[0] + bytes[1] * 256;
	// temperature
	rawHumid = bytes[2] + bytes[3] * 256;
	// humidity
	rawBat = bytes[4] + bytes[5] * 256;
	// vBat
	rawDevOk = bytes[6] + bytes[7] * 256;
	// likelihood that the Deffibrillator is showing a enabled Display
	rawCameraModulAttached = bytes[8];
	//
	rawLat = bytes[9] + bytes[10] * 256;
	//latitude of the Device
	rawLng = bytes[11] + bytes[12] * 256;
	//longitude of the Device
	devId = bytes[13];
  
	return {
	  vBat: sflt162f(rawBat) * 10,
	  humidity: sflt162f(rawHumid) * 100,
	  temperature: sflt162f(rawTemp) * 100,
	  cameraConnected: '1' === String.fromCharCode(rawCameraModulAttached),
	  probabilityOk: sflt162f(rawDevOk),
	  devId: devId,
	  location: [sflt162f(rawLng) * 180, sflt162f(rawLat) * 90]
	};
  }
  
  function sflt162f(rawSflt16) {
	// rawSflt16 is the 2-byte number decoded from wherever;
	// it's in range 0..0xFFFF
	// bit 15 is the sign bit
	// bits 14..11 are the exponent
	// bits 10..0 are the the mantissa. Unlike IEEE format,
	// 	the msb is transmitted; this means that numbers
	//	might not be normalized, but makes coding for
	//	underflow easier.
	// As with IEEE format, negative zero is possible, so
	// we special-case that in hopes that JavaScript will
	// also cooperate.
	//
	// The result is a number in the open interval (-1.0, 1.0);
	//
  
	// throw away high bits for repeatability.
	rawSflt16 &= 0xffff;
  
	// special case minus zero:
	if (rawSflt16 == 0x8000) return -0.0;
  
	// extract the sign.
	var sSign = (rawSflt16 & 0x8000) !== 0 ? -1 : 1;
  
	// extract the exponent
	var exp1 = (rawSflt16 >> 11) & 0xf;
  
	// extract the "mantissa" (the fractional part)
	var mant1 = (rawSflt16 & 0x7ff) / 2048.0;
  
	// convert back to a floating point number. We hope
	// that Math.pow(2, k) is handled efficiently by
	// the JS interpreter! If this is time critical code,
	// you can replace by a suitable shift and divide.
	var f_unscaled = sSign * mant1 * Math.pow(2, exp1 - 15);
  
	return f_unscaled;
  }
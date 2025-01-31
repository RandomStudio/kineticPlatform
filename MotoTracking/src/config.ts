const defaults = {
  appName: "MyApp",
  loglevel: "info",
  serial: {
    port: "/dev/tty.usbserial-6",
    baudrate: 9600,
  },
  waitToZero: 9000,
  flipDirection: true,
  testMode: false,
};

export default defaults;

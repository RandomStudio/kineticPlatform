import defaults from "./config";
import parse from "parse-strings-in-object";
import rc from "rc";
import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import { getLogger } from "log4js";
import { BROKER_DEFAULTS, decode, InputPlug, TetherAgent } from "tether-agent";
import { remap } from "@anselan/maprange";
import { TrackedPoint } from "./types";

const appName = defaults.appName;

const config: typeof defaults = parse(rc(appName, defaults));

const logger = getLogger(appName);
logger.level = config.loglevel;

logger.info("started with config", config);
logger.debug("Debug logging enabled; output could be verbose!");

const main = async () => {
  // Do some async stuff in here!

  const serialport = new SerialPort({
    path: config.serial.port,
    baudRate: config.serial.baudrate,
  });
  const parser = serialport.pipe(new ReadlineParser({ delimiter: "\r\n" }));
  parser.on("data", (ch) => logger.info(`incoming serial: ${ch}`));

  serialport.on("open", () => {
    serialport.write(0 + "\n");
  });

  const agent = await TetherAgent.create("trackingToMotor", {
    brokerOptions: BROKER_DEFAULTS.nodeJS,
  });
  const trackingInput = await InputPlug.create(agent, "smoothedRemappedPoints");
  trackingInput.on("message", (payload) => {
    const subjects = decode(payload) as TrackedPoint[];
    for (const u of subjects) {
      const { id, x, y } = u;
      const angleTo = toDegreees(Math.atan2(y, x));

      if (id === 0) {
        logger.debug({ id, angleTo });
        serialport.write(-angleTo + "\n");
      }
    }
  });

  // setInterval(() => {
  //   const angle = remap(Math.random(), [0, 1], [-180, 180]).toFixed(1);
  //   logger.debug("sending random angle", angle);
  //   serialport.write(angle + "\n");
  // }, 3000);
};

// ================================================
// Kick off main process here
main();

const toDegreees = (radians: number) => (radians * 180) / Math.PI;

// eslint-disable-next-line node/no-unsupported-features/node-builtins
const { parentPort } = require("worker_threads");

require("ts-node").register();

process.on("uncaughtException", (err) => {
  // eslint-disable-next-line no-console
  console.error("worker thread uncaughtException", err);
  if (parentPort) {
    parentPort.postMessage(err);
  }
});

require("./worker-thread-test");

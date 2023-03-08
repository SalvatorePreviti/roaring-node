// eslint-disable-next-line node/no-unsupported-features/node-builtins
import { Worker } from "worker_threads";
import { resolve as pathResolve } from "path";

// Check that node is 12 or higher
const nodeVersion = parseInt(process.versions.node.split(".")[0], 10);
if (nodeVersion >= 12) {
  describe("RoaringBitmap32 worker-threads", () => {
    it("can be used and works inside a worker thread", () => {
      const worker = new Worker(pathResolve(__dirname, "worker-thread-test.js"));
      return new Promise<void>((resolve, reject) => {
        worker.on("message", (message) => {
          if (message === "ok") {
            resolve();
          } else {
            reject(new Error(message));
          }
        });
        worker.on("error", reject);
        worker.on("exit", (code) => {
          if (code !== 0) {
            reject(new Error(`Worker stopped with exit code ${code}`));
          }
        });
      });
    });
  });
}

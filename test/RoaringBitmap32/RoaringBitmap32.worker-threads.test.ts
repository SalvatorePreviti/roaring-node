import { describe, it } from "vitest";

const { resolve: pathResolve } = require("node:path");

describe("RoaringBitmap32 worker-threads", () => {
  it("can be used and works inside a worker thread", () => {
    const { Worker } = require("node:worker_threads");
    const worker = new Worker(pathResolve(__dirname, "worker-thread-test.js"));
    return new Promise<void>((resolve, reject) => {
      worker.on("message", (message: any) => {
        if (message === "ok") {
          resolve();
        } else {
          reject(new Error(message));
        }
      });
      worker.on("error", reject);
      worker.on("exit", (code: any) => {
        if (code !== 0) {
          reject(new Error(`Worker stopped with exit code ${code}`));
        }
      });
    });
  });
});

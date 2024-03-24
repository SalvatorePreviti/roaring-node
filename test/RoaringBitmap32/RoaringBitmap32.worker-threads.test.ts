import { resolve as pathResolve } from "path";
import { describe, it, expect } from "vitest";

// Check that node is 12 or higher
describe("RoaringBitmap32 worker-threads", () => {
  it("can be used and works inside a worker thread", async () => {
    // eslint-disable-next-line node/no-unsupported-features/node-builtins
    const { Worker } = require("worker_threads");
    const worker = new Worker(pathResolve(__dirname, "worker-thread-test.js"));
    const promise = new Promise<boolean>((resolve, reject) => {
      worker.on("message", (message: any) => {
        if (message === "ok") {
          resolve(true);
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
    await expect(promise).resolves.toBe(true);
  });
});

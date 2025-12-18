import { spawn } from "node:child_process";
import { fileURLToPath } from "node:url";
import { defineConfig } from "vitest/config";

async function runSystemInfo() {
  const scriptPath = fileURLToPath(new URL("./scripts/system-info.js", import.meta.url));

  await new Promise((resolve, reject) => {
    const child = spawn(process.execPath, [scriptPath], { stdio: "inherit" });
    child.once("exit", (code) => {
      if (code === 0) {
        resolve();
      } else {
        reject(new Error(`system-info exited with code ${code}`));
      }
    });
    child.once("error", reject);
  });
}

await runSystemInfo();

export default defineConfig({
  test: {
    include: ["test/**/*.test.ts"],
    environment: "node",
    benchmark: {
      include: ["benchmarks/**/*.bench.ts"],
    },
  },
});

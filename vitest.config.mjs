import { spawn } from "node:child_process";
import { fileURLToPath } from "node:url";
import { defineConfig } from "vitest/config";

async function runSystemInfo() {
  const scriptPath = fileURLToPath(new URL("./scripts/system-info.js", import.meta.url));
  spawn(process.execPath, [scriptPath], { stdio: "inherit" });
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

import { defineConfig } from "vitest/config";

import { printSystemInfo } from "./scripts/system-info.js";

printSystemInfo();

export default defineConfig({
  test: {
    include: ["test/**/*.test.ts"],
    environment: "node",
  },
});
